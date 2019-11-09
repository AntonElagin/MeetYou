#include "http_session.h"
#include "tests/gmock/http_session.h"


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
  handle_request(parser->release(), queue);

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

template <class Body, class Allocator, class Send>
void http_session::handle_request(
    http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
  //    Тут будет парсинг url и перенаправление на response_request(request)
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

bool http_session::queue::is_full() const {
  return items.size() >= limit;
}


