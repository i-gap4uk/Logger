#ifndef CONTANTS_H
#define CONTANTS_H

#include <string>
namespace log_constants {

enum class LogLevel { INFO = 0, DEBUG, WARNING, ERROR };
enum class LogDestination { CONSOLE = 0, FILE };
constexpr const char* default_logfile_name = "logfile.log";

}  // namespace log_constants

#define INFO log_constants::LogLevel::INFO
#define DEBUG log_constants::LogLevel::DEBUG
#define WARNING log_constants::LogLevel::WARNING
#define ERROR log_constants::LogLevel::ERROR

#endif  // CONTANTS_H
