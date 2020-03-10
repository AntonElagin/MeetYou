#include "HttpSession.h"
#include <cppconn/exception.h>
#include "WebsocketSession.h"
#include "AuthMiddleware.h"
#include "Router.h"
#include "mysql_driver.h"
#include <stdexcept>

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
        try {
            sql::Driver *driver = get_driver_instance();
            std::shared_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "12A02El99"));
            conn->setSchema("MeetYou");
            auto req = parser->get();
            AuthMiddleware auth(conn, req, stream.socket().remote_endpoint().address().to_string());
            if (auth.isAuth()) {
                User user;
                user.userid = auth.getUserId();///получили id usera
                auto target = req.target().to_string();
                std::unique_ptr<sql::PreparedStatement> stmt(
                        conn->prepareStatement("select * from user where id=?"));
                stmt->setInt(1, user.userid);
                stmt->executeQuery();
                std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
                if (res->next())
                    user.username = res->getString("login");///вытащили логин из бд, пройдет тк is auth прошел
                std::smatch result;
                std::regex r_chat("/chat\\?id=(\\d+)");//гарантия что id число положительное
                int chatid = -1;
                if (std::regex_search(target, result, r_chat)) {
                    std::smatch::iterator iter = result.begin();
                    iter++;
                    chatid = std::stoi(*iter);
                } else {
                    throw std::invalid_argument("target must be like as /chat?id=14");
                }
                stmt.reset(conn->prepareStatement("select distinct chat_id,user_id from "
                                                  "result_table where chat_id=? and user_id=?"));

                stmt->setInt(1, chatid);
                stmt->setInt(2, user.userid);
                res.reset(stmt->executeQuery());
                if (!res->next())
                    throw "You haven't access permission to the chat";
                if (user.userid >= 0) {
                    stmt.reset(conn->prepareStatement("select * from result_table where user_id=? and chat_id=?"));
                    stmt->setInt(1, user.userid);
                    stmt->setInt(2, chatid);
                    res.reset(stmt->executeQuery());
                    if (res->next()) {
                        boost::make_shared<WebsocketSession>(stream.release_socket(), state, conn,
                                                             chatid, user)->run(parser->release());
                    } else {
                        throw "not found user in this chat";
                    }
                }
            }
        }
        catch (sql::SQLException &e) {
            json body({"SQL error", e.what()});
            ResponseCreator resp(body, 1);
            queue(std::move(resp.get_resp()));
        }
        catch (std::invalid_argument &e) {
            json body({"argument error", e.what()});
            ResponseCreator resp(body, 1);
            queue(std::move(resp.get_resp()));
        }
        catch (const char *e) {
            json body({"Error", e});
            ResponseCreator resp(body, 1);
            queue(std::move(resp.get_resp()));
        }
        return;
    }
    if (parser->get().target().back() == '/') {
        http::file_body::value_type body;
        std::string path = "../for_presentation/present.html";
        body.open(path.c_str(), beast::file_mode::scan, ec);
        http::response<http::file_body> res{
                std::piecewise_construct,
                std::make_tuple(std::move(body)),
                std::make_tuple(http::status::ok, parser->get().version())};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(parser->get().keep_alive());
        res.prepare_payload();
        queue(std::move(res));
    }
    // Отправляем ответ
    Router router(parser->release(), stream.socket().remote_endpoint().address().to_string());
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
