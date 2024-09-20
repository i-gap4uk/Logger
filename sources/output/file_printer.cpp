#include "output/file_printer.h"

#include "utils/constants.h"
namespace logger {
FilePrinter::FilePrinter() : LogPrinter(log_constants::LogDestination::FILE) {
  init();
}

FilePrinter::~FilePrinter() { shutdown(); }

void FilePrinter::log_message(const std::string &message) {
  std::lock_guard<std::mutex> lock(_file_mutex);
  _file_stream << message;
}

void FilePrinter::init() {
  std::lock_guard<std::mutex> lock(_file_mutex);
  _file_stream.open(log_constants::default_logfile_name);
  _file_stream << "[ Start logging ]" << std::endl << std::endl;
}

void FilePrinter::shutdown() {
  std::lock_guard<std::mutex> lock(_file_mutex);
  _file_stream << std::endl << "[ Stop logging ]";
  _file_stream.close();
}
}  // namespace logger
