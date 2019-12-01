#include "../include/listener.h"

listener::listener(net::io_context &_ioc, tcp::endpoint _endpoint)
    : ioc(_ioc),
      acceptor(net::make_strand(_ioc))

{
  beast::error_code ec;


  acceptor.open(_endpoint.protocol(), ec);
  if (ec) {
    fail(ec, "open");
    return;
  }

  // Разрешить повторное использование адресов
  acceptor.set_option(net::socket_base::reuse_address(true), ec);
  if (ec) {
    fail(ec, "set_option");
    return;
  }

  //Привязка к адресу сервера
  acceptor.bind(_endpoint, ec);
  if (ec) {
    fail(ec, "bind");
    return;
  }

  // Start listening for connections
  acceptor.listen(net::socket_base::max_listen_connections, ec);
  if (ec) {
    fail(ec, "listen");
    return;
  }
}

void listener::run() { do_accept(); }

void listener::do_accept() {
  acceptor.async_accept(
      net::make_strand(ioc),
      beast::bind_front_handler(&listener::on_accept, shared_from_this()));
}

void listener::on_accept(beast::error_code ec, tcp::socket socket) {
  if (ec) {
    fail(ec, "accept");
  } else {
    // Создаем сеанс http и запусткаем его
    std::make_shared<http_session>(std::move(socket))->run();
  }
  // Принять другое соединение
  do_accept();
}


