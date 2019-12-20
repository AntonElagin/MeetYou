#ifndef TECHPROJECT_HTTPSESSION_H
#define TECHPROJECT_HTTPSESSION_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "IHttpSession.h"
#include "Queue.h"
#include "ViewRegistration.h"
#include "WebsocketSession.h"
#include "Work.h"

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace websocket = boost::beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

// Обрабатывает соединение HTTP-сервера
class HttpSession : public std::enable_shared_from_this<HttpSession>,
                    public IHttpSession {
  //  // Эта очередь используется для конвейерной передачи HTTP.
  //  class queue {
  //    enum {
  //      // Максимальное количество ответов, которые мы будем ставить в очередь
  //      limit = 8
  //    };
  //
  //    HttpSession &self;
  //    std::vector<std::unique_ptr<Work>> items;
  //
  //   public:
  //    explicit queue(HttpSession &_self);
  //    queue();
  //    bool is_full() const;
  //
  //    bool on_write();
  //
  //    // Вызывается обработчиком HTTP для отправки ответа.
  //    template <bool isRequest, class Body, class Fields>
  //    void operator()(http::message<isRequest, Body, Fields> &&msg_);
  //  };

  beast::tcp_stream stream;
  beast::flat_buffer buffer;
  Queue<HttpSession> queue;
  // The parser is stored in an optional container so we can
  // construct it from scratch it at the beginning of each new message.
  boost::optional<http::request_parser<http::string_body>> parser;

 public:
  // Получаем сокет
  explicit HttpSession(tcp::socket&& socket);
  beast::tcp_stream& getStream();
  void run() override;

  void onWrite(bool close, beast::error_code ec,
               std::size_t bytes_transferred) override;

 private:
  void doRead();

  void onRead(beast::error_code ec, std::size_t bytes_transferred);

  void doClose();
};

#endif  // TECHPROJECT_HTTPSESSION_H
