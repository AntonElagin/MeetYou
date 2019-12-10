#include "HttpSession.h"
#include <cppconn/exception.h>
#include "AuthMiddleware.h"
#include "Router.h"
#include "mysql_driver.h"

HttpSession::HttpSession(tcp::socket&& socket)
    : stream(std::move(socket)), queue(*this) {}

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
    // Создание сеанса websocket, передача прав на
    //  сокет и HTTP-запрос.
    std::make_shared<WebsocketSession>(stream.release_socket())
        ->do_accept(parser->release());
    return;
  }
  // Отправляем ответ
  // TODO : заменить на роутинг
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

beast::tcp_stream& HttpSession::getStream() { return stream; }


