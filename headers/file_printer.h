#ifndef FILE_PRINTER_H
#define FILE_PRINTER_H

#include <fstream>
#include <mutex>

#include "log_printer.h"

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

#endif  // FILE_PRINTER_H
