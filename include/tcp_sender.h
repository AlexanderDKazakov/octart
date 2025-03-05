#pragma once
#include "interfaces/sender_int.h"

class TCPSender : public Sender {

  // not used: investigating.
  size_t send__(const void *data, size_t size) override;

public:
  void sendFile(const char *filename);
};
