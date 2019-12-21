#include "HttpSession.h"
#include <cppconn/exception.h>
#include "WebsocketSession.h"
#include "AuthMiddleware.h"
#include "Router.h"
#include "mysql_driver.h"

HttpSession::HttpSession(tcp::socket &&socket, boost::shared_ptr<SharedState> const &_state)
        : stream(std::move(socket)), queue(*this), state(_state) {

}

void HttpSession::run() { doRead(); }

void HttpSession::doRead() {
    // синтаксический анализатор для каждого сообщения
    parser.emplace();
    //ограничение размера в байтах
    parser->body_limit(10000);

    // Установка тайм-аут
    stream.expires_after(std::chrono::seconds(30));

    // Читаем запрос с помощью парсера
    http::async_read(
            stream, buffer, *parser,
            beast::bind_front_handler(&HttpSession::onRead, shared_from_this()));
}

void HttpSession::onRead(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec == http::error::end_of_stream) return doClose();

    if (ec) return fail(ec, "read");

    // Если websocket
    if (websocket::is_upgrade(parser->get())) {
        sql::Driver *driver = get_driver_instance();
        std::shared_ptr <sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "12A02El99"));
        conn->setSchema("MeetYou");
        auto req = parser->get();
        AuthMiddleware auth(conn, req, stream.socket().remote_endpoint().address().to_string());
        if (auth.isAuth()) {
            User user;
            user.userid = auth.getUserId();
            auto target = req.target().to_string();
            std::shared_ptr <sql::PreparedStatement> stmt(
                    conn->prepareStatement("select * from user where id=?"));
            stmt->setInt(1, user.userid);
            stmt->executeQuery();
            std::shared_ptr <sql::ResultSet> res(stmt->executeQuery());
            while (res->next()) {
                user.username = res->getString("login");
            }
            std::smatch result;
            std::regex r_chat("/chat\\?id=(\\d+)");
            int chatid = -1;
            if (std::regex_search(target, result, r_chat)) {
                std::smatch::iterator iter = result.begin();
                iter++;
                chatid = std::stoi(*iter);
            }
            ///проверили все права
            stmt.reset(conn->prepareStatement("select distinct chat_id,user_id from "
                                              "result_table where chat_id=? and user_id=?"));

            stmt->setInt(1, chatid);
            stmt->setInt(2, user.userid);
            res.reset(stmt->executeQuery());
            if (!res->next()) {
                std::cerr << "you don't have access to the chat" << std::endl;
                return;
            }

            if (user.userid >= 0) {
                stmt.reset(conn->prepareStatement("select * from result_table where user_id=? and chat_id=?"));
                stmt->setInt(1, user.userid);
                stmt->setInt(2, chatid);
                res.reset(stmt->executeQuery());
            };
            if (res->next()) {
                ///нет проверки валидности запроса(без куки упадет все)
                // of both the socket and the HTTP request.
                boost::make_shared<WebsocketSession>(stream.release_socket(), state, conn,
                                                     chatid, user)->run(parser->release());
            } else {
                std::cerr << "error with data" << std::endl;
                ///generate response access denied
            }

        } else {
            std::cerr << "not auth user" << std::endl;
        }
        return;
    }
    // Отправляем ответ

    Router router(parser->release(),
                  stream.socket().remote_endpoint().address().to_string());
    router.startRouting(queue);

    // Если мы не находимся на пределе очереди, попробуйте передать другой запрос
    // по конвейеру
    if (!queue.is_full()) doRead();
}

void HttpSession::onWrite(bool close, beast::error_code ec,
                          std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec) return fail(ec, "write");

    if (close) {
        // Закрываяем соединение
        return doClose();
    }

    // Inform the queue that a write completed
    if (queue.onWrite()) {
        // Read another request
        doRead();
    }
}

void HttpSession::doClose() {
    // Отправляем завершение работы
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_send, ec);
}

beast::tcp_stream &HttpSession::getStream() { return stream; }
