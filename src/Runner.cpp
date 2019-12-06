#include "Runner.h"
#include <utility>

Runner::Runner(net::ip::address _address, const unsigned int &_port,
               const int &_threads)
    : address(std::move(_address)), port(_port), threads(_threads) {}

void Runner::run() {
  // Io_context требуется для всех операций ввода-вывода
  boost::asio::io_context ioc{threads};

  // Создаем и запускаем порт прослушивания
  std::make_shared<Listener>(ioc, tcp::endpoint{address, port})->run();

  // Захват SIGINT и SIGTERM для выполнения чистого завершения работы
  boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
  signals.async_wait([&](beast::error_code const &, int) {
    // остановка в случаем получения сигнала завершение
    ioc.stop();
  });

  // Запускаем службу ввода-выводв
  // на нужном количестве портов
  std::vector<std::thread> v;
  v.reserve(threads - 1);
  for (auto i = threads - 1; i > 0; --i) v.emplace_back([&ioc] { ioc.run(); });
  ioc.run();

  // (If we get here, it means we got a SIGINT or SIGTERM)

  // Block until all the threads exit
  for (auto &t : v) t.join();
}