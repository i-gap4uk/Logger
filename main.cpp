#include <thread>

#include "log.h"
#include "scope_timer.h"

using namespace logger;

void f1() {
  auto logger1 = getLogger();

  logger1.add_log_destination(log_constants::LogDestination::FILE);
  logger1(DEBUG)
      << "Running a thread";  // also could be used LOG_DEBUG(logger1) << ...

  auto logger2 = getLogger("f1");
  logger2 << "Running a thread" << " but log to another logger";
}

void f2() {
  Logger logger{"f2"};
  logger.add_log_destination(log_constants::LogDestination::FILE);
  logger(DEBUG) << "Running a thread";

  const double timeSpent = 10.0;

  logger(WARNING) << "Time spent in the thread: " << timeSpent << " seconds";
}

void f3() {
  auto logger = getLogger("f3");
  logger << "Running a thread";

  const int i = 123;
  logger(ERROR) << "My int is " << i;
}

int main() {
  ScopeTimer t;

  auto logger = getLogger();
  LOG_INFO(logger) << "Starting the app";

  std::thread t1(f1);
  std::thread t2(f2);
  std::thread t3(f3);

  t1.join();
  t2.join();
  t3.join();

  return 0;
}
