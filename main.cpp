#include "config.h"
#include "tcp_receiver.h"
#include "tcp_sender.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage:\n"
              << "  Server: " << argv[0] << " -s\n"
              << "  Client: " << argv[0] << " -c path/to/the/file\n";
    return EXIT_FAILURE;
  }

  std::string mode = argv[1];

  if (mode == "-s") {
    std::unique_ptr<TCPServer> server = std::make_unique<TCPServer>(PORT);
    server->start();
    while (true) { // Keep the main thread alive
      std::this_thread::sleep_for(std::chrono::seconds(60));
    }

  } else if (mode == "-c" && argc == 3) {
    std::unique_ptr<TCPSender> sender = std::make_unique<TCPSender>();
    sender->sendFile(argv[2]);
  } else {
    std::cerr << "Invalid arguments.\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
