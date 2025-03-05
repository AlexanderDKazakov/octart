#pragma once
#include <cstddef>

class Sender {
public:
  virtual size_t send__(const void *data, size_t size) = 0;
  virtual ~Sender() = default;
};
