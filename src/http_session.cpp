#include "../include/http_session.h"

#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include "../include/auth_middleware.h"
#include "../include/view_registration.h"
#include "mysql_connection.h"
#include "mysql_driver.h"

template <class Send>
void handle_request1(const http::request<http::string_body>& req, Send&& send) {
  boost::string_view st;
  std::cout << req;

  //  st = req.at("Cookie");
  auto a = req.find("Cookie");
  http::response<http::string_body> res1;
  try {
    sql::ResultSet* res;
    sql::Driver* driver = get_driver_instance();
    std::shared_ptr<sql::Connection> con(
        driver->connect("tcp://127.0.0.1:3306", "root", "12A02El99"));
    con->setSchema("MeetYou");

    AuthMiddleware authMiddleware(con, st);
    authMiddleware.is_Auth();
    ViewRegistration viewRegistration(req, con);

    res1 = viewRegistration.get();

  } catch (sql::SQLException& e) {
    std::cout << e.what() << std::endl << "kek";
  }
  // Respond to GET request
  std::string body;
  return send(std::move(res1));
}

http_session::http_session(tcp::socket&& socket)
    : stream(std::move(socket)), queue(*this) {}

void http_session::run() { do_read(); }

void http_session::do_read() {
  // синтаксический анализатор для каждого сообщения
  parser.emplace();

  //ограничение размера в байтах
  parser->body_limit(10000);

  // Установка тайм-аут
  stream.expires_after(std::chrono::seconds(30));

  // Читаем запрос с помощью парсера
  http::async_read(
      stream, buffer, *parser,
      beast::bind_front_handler(&http_session::on_read, shared_from_this()));
}

void http_session::on_read(beast::error_code ec,
                           std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec == http::error::end_of_stream) return do_close();

  if (ec) return fail(ec, "read");

  // Если websocket
  if (websocket::is_upgrade(parser->get())) {
    // Создание сеанса websocket, передача прав на
    //  сокет и HTTP-запрос.
    std::make_shared<websocket_session>(stream.release_socket())
        ->do_accept(parser->release());
    return;
  }

  // Отправляем ответ
  // TODO : зааменить на роутинг
  //  handle_request(parser->release(), queue);
  handle_request1(parser->release(), queue);

  // Если мы не находимся на пределе очереди, попробуйте передать другой запрос
  // по конвейеру
  if (!queue.is_full()) do_read();
}

void http_session::on_write(bool close, beast::error_code ec,
                            std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec) return fail(ec, "write");

  if (close) {
    // Закрываяем соединение
    return do_close();
  }

  // Inform the queue that a write completed
  if (queue.on_write()) {
    // Read another request
    do_read();
  }
}

void http_session::do_close() {
  // Отправляем завершение работы
  beast::error_code ec;
  stream.socket().shutdown(tcp::socket::shutdown_send, ec);
}

http_session::queue::queue(http_session& _self) : self(_self) {
  static_assert(limit > 0, "queue limit must be positive");
  items.reserve(limit);
}

bool http_session::queue::on_write() {
  BOOST_ASSERT(!items.empty());
  auto const was_full = is_full();
  items.erase(items.begin());
  if (!items.empty()) (*items.front())();
  return was_full;
}

template <bool isRequest, class Body, class Fields>
void http_session::queue::operator()(
    http::message<isRequest, Body, Fields>&& msg_) {
  // This holds a work item
  struct work_impl : work {
    http_session& self;
    http::message<isRequest, Body, Fields> msg;

    work_impl(http_session& self, http::message<isRequest, Body, Fields>&& _msg)
        : self(self), msg(std::move(_msg)) {}

    void operator()() {
      http::async_write(
          self.stream, msg,
          beast::bind_front_handler(&http_session::on_write,
                                    self.shared_from_this(), msg.need_eof()));
    }
  };

  // Allocate and store the work
  items.push_back(boost::make_unique<work_impl>(self, std::move(msg_)));

  // If there was no previous work, start this one
  if (items.size() == 1) (*items.front())();
}

bool http_session::queue::is_full() const { return items.size() >= limit; }
