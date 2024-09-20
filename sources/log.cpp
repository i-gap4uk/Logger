#include "log.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "console_printer.h"
#include "file_printer.h"

using namespace log_constants;

namespace {
std::string log_level_to_string(LogLevel log_level) {
  switch (log_level) {
    case INFO:
      return std::string(" INFO  ");
    case DEBUG:
      return std::string(" DEBUG ");
    case WARNING:
      return std::string("WARNING");
    case ERROR:
      return std::string(" ERROR ");
    default:
      return std::string("UNKNOWN");
  }
}

constexpr const char *RESET = "\033[0m";

// Foreground color codes
constexpr const char *RED = "\033[31m";
constexpr const char *GREEN = "\033[32m";
constexpr const char *YELLOW = "\033[33m";
constexpr const char *BLUE = "\033[34m";

std::string color_text(const std::string &text, LogLevel log_level) {
  std::string colored_text;

  switch (log_level) {
    case INFO:
      colored_text.append(BLUE);
      break;
    case DEBUG:
      colored_text.append(GREEN);
      break;
    case WARNING:
      colored_text.append(YELLOW);
      break;
    case ERROR:
      colored_text.append(RED);
      break;
    default:
      return text;
  }

  colored_text.append(text);
  colored_text.append(RESET);

  return colored_text;
}

std::string assemble_log_message(const std::string &message,
                                 const std::string prefix, LogLevel log_level) {
  std::ostringstream stream;

  auto now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  stream << std::put_time(std::localtime(&now_time), "[%d/%m/%Y] [%H:%M:%S]");
  stream << " ";

  stream << "[" << color_text(log_level_to_string(log_level), log_level)
         << "] ";

  if (!prefix.empty()) {
    stream << prefix;
  }

  stream << "[" << std::this_thread::get_id() << "]";
  stream << ": ";
  stream << message << std::endl;

  return stream.str();
}

std::shared_ptr<logger::LogPrinter> create_printer(LogDestination destination) {
  switch (destination) {
    case log_constants::LogDestination::CONSOLE:
      return std::make_shared<logger::ConsolePrinter>();
    case LogDestination::FILE:
      return std::make_shared<logger::FilePrinter>();
      ;
      break;
    default:
      return {};
  }
}

std::string getMethodName(const std::string &full_method_name) {
  std::string tmp = full_method_name;
  size_t colonPos = tmp.find("::");
  if (colonPos != std::string::npos) {
    tmp.erase(0, colonPos + 2);
  } else {
    size_t spacePos = tmp.find(' ');
    if (spacePos != std::string::npos) {
      tmp.erase(0, spacePos + 1);
    }
  }
  size_t parenPos = tmp.find('(');
  if (parenPos != std::string::npos) {
    tmp.erase(parenPos);
  }
  return tmp;
}

std::string getClassName(const std::string &full_method_name) {
  std::string tmp = full_method_name;
  size_t colonPos = tmp.find("::");
  if (colonPos != std::string::npos) {
    tmp.erase(colonPos);
    size_t spacePos = tmp.find_last_of(' ');
    if (spacePos != std::string::npos) {
      tmp.erase(0, spacePos + 1);
    }
    return tmp;
  }
  tmp.erase(0, tmp.length());
  return tmp;
}

}  // namespace

namespace logger {
Logger::Accumulator::Accumulator(Logger *logger) : _logger(logger) {}

Logger::Accumulator::Accumulator(const Accumulator &other) {
  this->_stream.clear();
  this->_stream << other._stream.str();
  this->_logger = other._logger;
}

Logger::Accumulator::~Accumulator() {
  {
    std::lock_guard<std::mutex> lock(_logger->_queue_mtx);
    _logger->_message_queue.push(assemble_log_message(
        _stream.str(), _logger->_prefix, _logger->_log_level));
    _logger->_cv.notify_one();
  }
}

LogPrinters Logger::_printers;
std::mutex Logger::_printers_mtx;
CustomPrinters Logger::_custom_printers;
std::mutex Logger::_custom_printers_mtx;

Logger::Logger(const std::string &prefix)
    : _isRunning(false), _log_level(INFO), _prefix(prefix) {
  add_log_destination(LogDestination::CONSOLE);
  startQueueProcessing();
}

Logger::Logger(const Logger &other) {
  _prefix = other._prefix;
  _log_level = other._log_level;
}

Logger::~Logger() { stopQueueProcessing(); }

Logger::Accumulator Logger::operator()(LogLevel log_level) {
  _log_level = log_level;
  return Accumulator(this);
}

void Logger::add_log_destination(log_constants::LogDestination destination) {
  {
    std::lock_guard<std::mutex> lock(_selected_printers_mtx);
    auto it = std::find_if(_selected_printers.begin(), _selected_printers.end(),
                           [destination](LogPrinterPtr printer) {
                             return printer->destination_type() == destination;
                           });
    if (it != _selected_printers.end()) {
      // the specified log destination already exists
      return;
    }
  }

  std::lock_guard<std::mutex> lock(_printers_mtx);
  if (_printers.find(destination) == _printers.end()) {
    _printers[destination] = create_printer(destination);
  }

  {
    std::lock_guard<std::mutex> lock(_selected_printers_mtx);
    _selected_printers.push_back(_printers[destination]);
  }
}

void Logger::remove_log_destination(log_constants::LogDestination destination) {
  std::lock_guard<std::mutex> lock(_selected_printers_mtx);

  auto it = std::find_if(_selected_printers.begin(), _selected_printers.end(),
                         [destination](LogPrinterPtr printer) {
                           return printer->destination_type() == destination;
                         });
  if (it == _selected_printers.end()) {
    // the specified log destination is no presented
    return;
  }

  _selected_printers.erase(
      std::remove_if(_selected_printers.begin(), _selected_printers.end(),
                     [&destination](LogPrinterPtr printer) {
                       return printer->destination_type() == destination;
                     }));
}

void Logger::add_log_printer(LogPrinterPtr log_printer) {
  std::lock_guard<std::mutex> lock(_custom_printers_mtx);
  _custom_printers.push_back(log_printer);
}

void Logger::startQueueProcessing() {
  {
    std::lock_guard<std::mutex> lock(_queue_mtx);
    if (!_isRunning) {
      _isRunning = true;
      _queue_processing_thread = std::thread(&Logger::processQueue, this);
    }
  }
}

void Logger::stopQueueProcessing() {
  {
    std::lock_guard<std::mutex> lock(_queue_mtx);
    _isRunning = false;
    _cv.notify_all();
  }

  if (_queue_processing_thread.joinable()) {
    _queue_processing_thread.join();
  }
}

void Logger::processQueue() {
  while (_isRunning) {
    std::unique_lock<std::mutex> lock(_queue_mtx);
    _cv.wait(lock, [this]() {
      return !_message_queue.empty() || !_isRunning;
      ;
    });

    while (!_message_queue.empty()) {
      std::string message = _message_queue.front();
      _message_queue.pop();
      {
        // use predefined printers
        std::lock_guard<std::mutex> lock(_selected_printers_mtx);
        for (const auto &printer : _selected_printers) {
          printer->log_message(message);
        }
      }
      {
        // use custom printers added by user
        std::lock_guard<std::mutex> lock(_custom_printers_mtx);
        for (const auto &printer : _custom_printers) {
          printer->log_message(message);
        }
      }
    }

    if (!_isRunning && _message_queue.empty()) {
      break;
    }
  }

  // in the case when processing is stopped, but the queue still contains some
  // messages
  while (!_message_queue.empty()) {
    std::string message = _message_queue.front();
    _message_queue.pop();
    {
      std::lock_guard<std::mutex> lock(_selected_printers_mtx);
      for (const auto &printer : _selected_printers) {
        printer->log_message(message);
      }
    }
  }
}

Logger getLogger(const std::string &prefix) { return Logger(prefix); }

std::string extract_method_name(const std::string &prettyFunction) {
  std::stringstream ss;
  ss << getClassName(prettyFunction) << "::" << getMethodName(prettyFunction);
  return ss.str();
}

}  // namespace logger
