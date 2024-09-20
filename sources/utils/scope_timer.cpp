#include "utils/scope_timer.h"

#include <iostream>

ScopeTimer::ScopeTimer() { _start = std::chrono::high_resolution_clock::now(); }

ScopeTimer::~ScopeTimer() {
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - _start;
  std::cout << std::endl << "Completed time: " << duration.count() << std::endl;
}
