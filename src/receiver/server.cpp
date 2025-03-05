#include "tcp_receiver.h"

#include "config.h"
#include "crc32.h"
#include "frame.h"

#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

void TCPServer::start() {
  if (running) {
    std::cout << "Server is already running!" << std::endl;
    return;
  }

  serverSocket = createServerSocket();
  if (serverSocket < 0) {
    std::cerr << "Failed to create server socket!" << std::endl;
    return;
  }

  running = true;
  std::cout << "Server is running..." << std::endl;

  serverThread = std::thread(&TCPServer::runServer, this);
}

void TCPServer::stop() {
  if (!running) {
    std::cout << "Server is not running!" << std::endl;
    return;
  }

  running = false;
  std::cout << "Stopping server..." << std::endl;

  close(serverSocket); // Close server socket and notify thread to exit
  serverThread.join(); // Wait for the server thread to finish
  std::cout << "Server stopped." << std::endl;
}

int TCPServer::createServerSocket() {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    return -1;
  }

  sockaddr_in serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(serverPort);

  if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    perror("bind");
    close(sock);
    return -1;
  }

  if (listen(sock, 5) < 0) {
    perror("listen");
    close(sock);
    return -1;
  }

  return sock;
}

void TCPServer::runServer() {
  sockaddr_in clientAddr;
  socklen_t clientLen = sizeof(clientAddr);

  while (running) {
    int clientSocket =
        accept(serverSocket, (sockaddr *)&clientAddr, &clientLen);
    if (clientSocket < 0) {
      perror("accept");
      continue;
    }

    // Start a new thread to handle the client
    std::thread(&TCPServer::handleClient, this, clientSocket).detach();
  }

  // Clean up and close the server socket when exiting the loop
  close(serverSocket);
}

void TCPServer::handleClient(int clientSocket) {
  FileTransferHeader header;
  if (recv(clientSocket, &header, sizeof(header), MSG_WAITALL) !=
      sizeof(header)) {
    std::cerr << "Header receiving failed!" << std::endl;
    close(clientSocket);
    return;
  }

  time_t now = time(nullptr);
  char filenameWithTime[512];
  struct tm *tm_info;
  tm_info = localtime(&now);
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  snprintf(filenameWithTime, sizeof(filenameWithTime),
           "%02d_%02d_%04d_%02d_%02d_%03ld.hex", tm_info->tm_mday,
           tm_info->tm_mon + 1, tm_info->tm_year + 1900, tm_info->tm_min,
           tm_info->tm_sec, ts.tv_nsec / 1000000);

  std::ofstream file(filenameWithTime, std::ios::binary | std::ios::trunc);
  if (!file) {
    std::cerr << "File saving failed." << std::endl;
    close(clientSocket);
    return;
  }

  char buffer[BUFFER_SIZE];
  uint64_t received = 0;
  std::cout << "Start saving!" << std::endl;
  while (received < header.filesize) {
    size_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
      std::cerr << "Connection interrupted!" << std::endl;
      file.close();
      close(clientSocket);
      std::remove(filenameWithTime);
      return;
    }
    file.write(buffer, bytesRead);
    received += bytesRead;
    std::cout << "Saved " << received << " bytes." << std::endl;
  }
  file.close();

  std::ifstream receivedFile(filenameWithTime,
                             std::ios::binary | std::ios::ate);
  if (!receivedFile || receivedFile.tellg() != header.filesize) {
    std::cerr << "Size check failed!" << std::endl;
    std::remove(filenameWithTime);
    close(clientSocket);
    return;
  }

  receivedFile.seekg(0, std::ios::beg);
  std::vector<char> data(header.filesize);
  receivedFile.read(data.data(), header.filesize);
  receivedFile.close();

  // Uncomment the following block if you need to verify CRC
  /*
  uint32_t calculatedCRC = calculateCRC32(data.data(), data.size());
  if (calculatedCRC != header.crc32) {
      std::cerr << "CRC32 error! File corrupted." << std::endl;
      std::remove(filenameWithTime);
      close(clientSocket);
      return;
  }
  */

  std::cout << "File is saved with success: " << filenameWithTime << std::endl;
  close(clientSocket);
}
