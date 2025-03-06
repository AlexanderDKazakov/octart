#include "tcp_receiver.h"
#include <gtest/gtest.h>

TEST(MainFunc, TCPSerTest) {
  std::unique_ptr<TCPServer> server = std::make_unique<TCPServer>(12345);
  EXPECT_EQ(server->get_port(), 12345);
  server->start();
  EXPECT_EQ(server->is_running(), true);
  std::this_thread::sleep_for(std::chrono::seconds(3));
  server->stop();
  EXPECT_EQ(42, 42);
}

TEST(MainFunc, Test2) {
  EXPECT_NE(1, 2); // Another fancy test
}
