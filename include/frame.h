#pragma once

#include <cstdint>

struct FileTransferHeader {
  char filename[256];
  uint64_t filesize;
  /*uint32_t crc32;*/
};
