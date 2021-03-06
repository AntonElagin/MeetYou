#include "WebsocketSession.h"
#include "boost/date_time/posix_time/posix_time.hpp"

WebsocketSession::WebsocketSession(tcp::socket &&socket, boost::shared_ptr<SharedState> const &state,
                                   std::shared_ptr<sql::Connection> conn, int chatid, User user)
        : ws_(std::move(socket)), state_(state), chatid(chatid), conn(conn), user(user) {}

WebsocketSession::~WebsocketSession() {
    // Remove this session from the list of active sessions
    state_->leave(this, chatid);
}

void WebsocketSession::fail(beast::error_code ec, char const *what) {
    // Don't report these
    if (ec == net::error::operation_aborted ||
        ec == websocket::error::closed)
        return;
    std::cerr << what << ": " << ec.message() << "\n";
}

void WebsocketSession::on_accept(beast::error_code ec) {
    // Handle the error, if any
    if (ec)
        return fail(ec, "accept");
    // Add this session to the list of active sessions
    ///load message from db
    std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "select login nick,body,publication_date pubdate from message, user where chat_id = ? and author_id = user.id order by publication_date asc;"));
    stmt->setInt(1, chatid);
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
    std::string text;
    std::string author;
    std::string date;
    std::string buffer;///если пусто в бд то ничего не вернет
    while (res->next()) {///load history from db
        text = res->getString("body");
        author = res->getString("nick");
        date = res->getString("pubdate");
        buffer = date + " : " + author + " : " + text;
        auto const ss = boost::make_shared<std::string const>(std::move(buffer));
        send(ss);///вренули текущему юзеру
    }
    state_->join(this, chatid);
    // Read a message
    ws_.async_read(buffer_, beast::bind_front_handler(&WebsocketSession::on_read, shared_from_this()));
}

void WebsocketSession::on_read(beast::error_code ec, std::size_t) {
    // Handle the error, if any
    if (ec) return fail(ec, "read");
    boost::posix_time::ptime timeLocal = boost::posix_time::second_clock::local_time();
    auto message = beast::buffers_to_string(buffer_.data());
    auto message_text_ws = boost::posix_time::to_simple_string(timeLocal) + " : " + user.username + " : " +
                           beast::buffers_to_string(buffer_.data());
    // Send to all connections
    std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "INSERT INTO `MeetYou`.`message` (`publication_date`, `body`, `author_id`, `chat_id`) VALUES (DEFAULT, ?, ?, ?)"));
    ///пройдет тк id разные у сообщений
    stmt->setString(1, message);
    stmt->setInt(2, user.userid);
    stmt->setInt(3, chatid);
    stmt->executeUpdate();
    state_->send(message_text_ws, chatid);///расшарить всем остальным клиентам
    // Clear the buffer
    buffer_.consume(buffer_.size());
    // Read another message
    ws_.async_read(buffer_, beast::bind_front_handler(&WebsocketSession::on_read, shared_from_this()));
}


void WebsocketSession::send(boost::shared_ptr<std::string const> const &ss) {
    // Post our work to the strand, this ensures
    // that the members of `this` will not be
    // accessed concurrently.
    net::post(ws_.get_executor(), beast::bind_front_handler(&WebsocketSession::on_send, shared_from_this(), ss));
}


void WebsocketSession::on_send(boost::shared_ptr<std::string const> const &ss) {
    // Always add to queue
    queue_.push_back(ss);
    // Are we already writing?
    if (queue_.size() > 1)
        return;
    // We are not currently writing, so send this immediately
    ws_.async_write(net::buffer(*queue_.front()),
                    beast::bind_front_handler(&WebsocketSession::on_write, shared_from_this()));
}

void WebsocketSession::on_write(beast::error_code ec, std::size_t) {
    // Handle the error, if any
    if (ec)
        return fail(ec, "write");
    // Remove the string from the queue
    queue_.erase(queue_.begin());
    // Send the next message if any
    if (!queue_.empty())
        ws_.async_write(net::buffer(*queue_.front()),
                        beast::bind_front_handler(&WebsocketSession::on_write, shared_from_this()));
}