#include "console_printer.h"

#include <iostream>

ConsolePrinter::ConsolePrinter()
    : LogPrinter(log_constants::LogDestination::CONSOLE) {}

void ConsolePrinter::log_message(const std::string &message) {
  std::cout << message;
}
