#pragma once

// Not standard
#include "interfaces/receiver_int.h"

// Standard
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

class TCPServer : public Receiver {
public:
  TCPServer(int port) : serverPort(port), running(false) {}
  void start() override;
  void stop() override;

  ~TCPServer() {
    if (running) {
      stop();
    }
  }

  int get_port() { return this->serverPort; }
  bool is_running() { return this->running.load(); }

private:
  int serverSocket;
  int serverPort;
  std::atomic<bool> running;
  std::thread serverThread;
  // handlers
  void runServer();
  void handleClient(int clientSocket);
  int createServerSocket();
};
