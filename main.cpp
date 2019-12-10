#include "listener.hpp"
#include "shared_state.hpp"

#include <boost/asio/signal_set.hpp>
#include <boost/smart_ptr.hpp>
#include <iostream>
#include <vector>

int
main(int argc, char *argv[]) {
    // Check command line arguments.
    if (argc != 5) {
        std::cerr <<
                  "Usage: websocket-chat-multi <address> <port> <doc_root> <threads>\n" <<
                  "Example:\n" <<
                  "    websocket-chat-server 0.0.0.0 8080 . 5\n";
        return EXIT_FAILURE;
    }
    auto address = net::ip::make_address(argv[1]);///сделали адрес из первог оаргумента
    auto port = static_cast<unsigned short>(std::atoi(argv[2]));///порт
    auto doc_root = argv[3];///корень
    auto const threads = std::max<int>(1, std::atoi(argv[4]));///кол-во потоков

    // The io_context is required for all I/O
    net::io_context ioc;

    // Create and launch a listening port
    auto listening = boost::make_shared<listener>(ioc, tcp::endpoint{address, port},
                                                  boost::make_shared<shared_state>(doc_root));
    listening->run();
///сделали и связали слушающий порт , книули ему ендпоинт +корень
    // Capture SIGINT and SIGTERM to perform a clean shutdown
    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(
            [&ioc](boost::system::error_code const &, int) {
                // Stop the io_context. This will cause run()
                // to return immediately, eventually destroying the
                // io_context and any remaining handlers in it.
                ioc.stop();
            });

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;///масив потоков
    v.reserve(threads - 1);///потоки
    for (auto i = threads - 1; i > 0; --i)
        v.emplace_back([&ioc] { ioc.run(); });///запустили иос
    ioc.run();
    // (If we get here, it means we got a SIGINT or SIGTERM)
    // Block until all the threads exit
    for (auto &t : v)
        t.join();

    return EXIT_SUCCESS;
}