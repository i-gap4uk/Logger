#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class ScopeTimer {
 public:
  ScopeTimer();
  ~ScopeTimer();

 private:
  std::chrono::high_resolution_clock::time_point _start;
};

#endif  // TIMER_H
