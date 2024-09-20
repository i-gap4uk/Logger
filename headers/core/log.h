#ifndef LOG_H
#define LOG_H

#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "utils/constants.h"

namespace logger {
class LogPrinter;
class Logger;

using LogPrinterPtr = std::shared_ptr<LogPrinter>;
using LogPrinters = std::map<log_constants::LogDestination, LogPrinterPtr>;
using SelectedPrinters = std::vector<LogPrinterPtr>;
using CustomPrinters = std::vector<LogPrinterPtr>;

class Logger {
 public:
  class Accumulator {
   public:
    Accumulator(Logger* logger);
    Accumulator(const Accumulator& other);
    template <typename T>
    std::stringstream& operator<<(const T& value) {
      _stream << value;
      return _stream;
    }

    ~Accumulator();

   private:
    std::stringstream _stream;
    Logger* _logger;
  };

  Logger(const std::string& prefix = "");
  Logger(const Logger& other);
  ~Logger();

  Accumulator operator()(log_constants::LogLevel log_level);

  template <typename T>
  Accumulator operator<<(const T& value) {
    Accumulator temp = (*this)(_log_level);
    temp << value;
    return temp;
  }

  void add_log_destination(log_constants::LogDestination destination);
  void remove_log_destination(log_constants::LogDestination destination);
  void add_log_printer(LogPrinterPtr log_printer);

 private:
  void startQueueProcessing();
  void stopQueueProcessing();
  void processQueue();

 private:
  log_constants::LogLevel _log_level;
  std::string _prefix;

  static LogPrinters _printers;
  static std::mutex _printers_mtx;

  static CustomPrinters _custom_printers;
  static std::mutex _custom_printers_mtx;

  SelectedPrinters _selected_printers;
  std::mutex _selected_printers_mtx;

  std::atomic<bool> _isRunning;
  std::queue<std::string> _message_queue;
  std::mutex _queue_mtx;
  std::condition_variable _cv;
  std::thread _queue_processing_thread;
};

Logger getLogger(const std::string& prefix = "");

std::string extract_method_name(const std::string& prettyFunction);

#define LOG_WITH_LOG_LEVEL(log_level, logger)                          \
  logger(log_level) << "[" << logger::extract_method_name(__PRETTY_FUNCTION__) \
                    << "]: " << __FILE__ << ":" << __LINE__ << " "

#define LOG_INFO(logger) LOG_WITH_LOG_LEVEL(INFO, logger)
#define LOG_DEBUG(logger) LOG_WITH_LOG_LEVEL(DEBUG, logger)
#define LOG_WARNING(logger) LOG_WITH_LOG_LEVEL(WARNING, logger)
#define LOG_ERROR(logger) LOG_WITH_LOG_LEVEL(ERROR, logger)

}  // namespace logger
#endif  // LOG_H
