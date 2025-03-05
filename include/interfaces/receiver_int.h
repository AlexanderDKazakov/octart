#pragma once

class Receiver {
public:
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual ~Receiver() = default;
};
