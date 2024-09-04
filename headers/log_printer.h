#ifndef LOG_PRINTER_H
#define LOG_PRINTER_H

#include <string>

#include "utils/constants.h"

class LogPrinter {
 public:
  LogPrinter(log_constants::LogDestination destination)
      : _destination(destination) {}
  virtual void log_message(const std::string& message) = 0;
  virtual ~LogPrinter() {}

  log_constants::LogDestination destination_type() const {
    return _destination;
  }

 private:
  log_constants::LogDestination _destination;
};

#endif  // LOG_PRINTER_H
