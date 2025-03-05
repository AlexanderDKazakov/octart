#include "config.h"
#include "crc32.h"
#include "frame.h"

#include <arpa/inet.h>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "tcp_sender.h"

size_t TCPSender::send__(const void *data, size_t size) { return 0; }
//
//   int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
//   if (clientSocket < 0) {
//     perror("socket");
//     return 0;
//   }
//
//   sockaddr_in serverAddr{};
//   serverAddr.sin_family = AF_INET;
//   serverAddr.sin_port = htons(PORT);
//   if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
//     perror("inet_pton");
//     close(clientSocket);
//     return 0;
//   }
//
//   if (connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
//   {
//     perror("connect");
//     close(clientSocket);
//     return 0;
//   }
//
//   size_t totalSent = 0;
//   while (totalSent < size) {
//     size_t sentBytes =
//         ::send(clientSocket, static_cast<const char *>(data) + totalSent,
//                std::min<size_t>(BUFFER_SIZE, size - totalSent), 0);
//     if (sentBytes <= 0) {
//       perror("send");
//       close(clientSocket);
//       return totalSent; // Return number of bytes sent so far
//     }
//     totalSent += sentBytes;
//   }
//
//   close(clientSocket);
//   return totalSent; // Return the total number of bytes sent
// }

// void sendFile_old(const char *filename) {
//   std::ifstream file(filename, std::ios::binary | std::ios::ate);
//   if (!file) {
//     std::cerr << "Failed to open file: " << filename << std::endl;
//     return;
//   }
//
//   uint64_t filesize = file.tellg();
//   file.seekg(0, std::ios::beg);
//
//   std::vector<char> data(filesize);
//   file.read(data.data(), filesize);
//   file.close();
//
//   // Prepare and send file transfer header
//   FileTransferHeader header{};
//   strncpy(header.filename, filename, sizeof(header.filename) - 1);
//   header.filesize = filesize;
//   /*header.crc32 = calculateCRC32(data.data(), data.size());*/
//
//   if (this->send__(&header, sizeof(header)) != sizeof(header)) {
//     std::cerr << "Failed to send file header!" << std::endl;
//     return;
//   }
//
//   // Send file data in chunks
//   size_t sent = 0;
//   while (sent < filesize) {
//     size_t chunkSize = std::min<size_t>(BUFFER_SIZE, filesize - sent);
//     size_t sentBytes = this->send__(data.data() + sent, chunkSize);
//     if (sentBytes <= 0) {
//       std::cerr << "Failed to send file data!" << std::endl;
//       return;
//     }
//     sent += sentBytes;
//   }
//
//   std::cout << "File sent successfully: " << filename << std::endl;
// }

void TCPSender::sendFile(const char *pfname) {
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0) {
    perror("socket");
    return;
  }

  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
    perror("inet_pton");
    close(clientSocket);
    return;
  }

  if (connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    perror("connect");
    close(clientSocket);
    return;
  }

  std::ifstream file(pfname, std::ios::binary | std::ios::ate);
  if (!file) {
    std::cerr << "Failed to open file: " << pfname << std::endl;
    close(clientSocket);
    return;
  }

  uint64_t filesize = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> data(filesize);
  file.read(data.data(), filesize);
  file.close();

  FileTransferHeader header{};
  strncpy(header.filename, pfname, sizeof(header.filename) - 1);
  header.filesize = filesize;
  /*header.crc32 = calculateCRC32(data.data(), data.size());*/

  if (send(clientSocket, &header, sizeof(header), 0) != sizeof(header)) {
    std::cerr << "Header send failed!" << std::endl;
    close(clientSocket);
    return;
  }

  size_t sent = 0;
  while (sent < filesize) {
    size_t sentBytes = send(clientSocket, data.data() + sent,
                            std::min<size_t>(BUFFER_SIZE, filesize - sent), 0);
    if (sentBytes <= 0) {
      perror("send");
      close(clientSocket);
      return;
    }
    sent += sentBytes;
  }

  std::cout << "File is sent with success: " << pfname << std::endl;
  close(clientSocket);
}
