#ifndef CONSOLE_PRINTER_H
#define CONSOLE_PRINTER_H

#include <log_printer.h>

#include <mutex>
namespace logger {
class ConsolePrinter : public LogPrinter {
 public:
  ConsolePrinter();
  void log_message(const std::string& message) override;
  ~ConsolePrinter() override = default;
};
}  // namespace logger
#endif  // CONSOLE_PRINTER_H
