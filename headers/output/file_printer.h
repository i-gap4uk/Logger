#ifndef FILE_PRINTER_H
#define FILE_PRINTER_H

#include <fstream>
#include <mutex>

#include "interfaces/log_printer.h"

namespace logger {
class FilePrinter : public LogPrinter {
 public:
  FilePrinter();
  ~FilePrinter();
  void log_message(const std::string &message) override;

 private:
  void init();
  void shutdown();

 private:
  std::mutex _file_mutex;
  std::ofstream _file_stream;
};
}  // namespace logger
#endif  // FILE_PRINTER_H
