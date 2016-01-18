//
// Created by Can on 1/12/16.
//

#ifndef HSISOMAP_LOGGER_H
#define HSISOMAP_LOGGER_H

#include <sstream>
#include <fstream>

namespace hsisomap_logger {

#define LOGFILE(file_path)                              \
hsisomap_logger::Logger::SetLogFile(file_path);

#define LOGSTDOUT(status)                               \
hsisomap_logger::Logger::SetLogToStdOut(status);

#define LOG(message)                                    \
hsisomap_logger::Logger::LogSimple(static_cast<std::ostringstream&>(     \
  std::ostringstream().flush() << message).str()        \
);

#define LOGNNL(message)                                 \
hsisomap_logger::Logger::LogSimple(static_cast<std::ostringstream&>(     \
  std::ostringstream().flush() << message).str(),       \
  false                                                 \
);

#define LOGTIMESTAMP(message)                           \
hsisomap_logger::Logger::Log(message,                                    \
  true, true, "[%Y-%m-%d %X %Z]",                       \
  false, true, "", "", "", "", 0                       \
);

#define LOGI(message)                                   \
hsisomap_logger::Logger::Log(static_cast<std::ostringstream&>(           \
  std::ostringstream().flush() << message).str(),       \
  true, true, "", false, true, "[INF]", "", "", "", 0   \
);

#define LOGE(message)                                   \
hsisomap_logger::Logger::Log(static_cast<std::ostringstream&>(           \
  std::ostringstream().flush() << message).str(),       \
  true, true, "", false, true, "[ERR]",                 \
  "\033[1;31m",                                         \
  "", "", 0                                             \
);

#define LOGW(message)                                   \
hsisomap_logger::Logger::Log(static_cast<std::ostringstream&>(           \
  std::ostringstream().flush() << message).str(),       \
  true, true, "", false, true, "[WRN]",                 \
  "\033[1;33m",                                         \
  "", "", 0                                             \
);

#define LOGR(message)                                   \
hsisomap_logger::Logger::Log(static_cast<std::ostringstream&>(           \
  std::ostringstream().flush() << message).str(),       \
  true, true, "", false, true, "[RES]",                 \
  "\033[1;32m",                                         \
  "", "", 0                                             \
);

#ifdef NDEBUG
#define LOGDEBUG(_) while (0) { }
#else
#define LOGDEBUG(message)                               \
hsisomap_logger::Logger::Log(static_cast<std::ostringstream&>(           \
  std::ostringstream().flush() << message).str(),       \
  true, true, "", true, true, "[DBG]",                 \
  "\033[1;36m",                                         \
  __FUNCTION__, __FILE__, __LINE__                      \
);
#endif


class Logger {
 public:
  ~Logger();
  static void Log(const std::string message, bool time_stamp, bool local_time, std::string time_format, bool show_source_location, bool new_line, const std::string tag, const std::string prefix, char const* function, char const* file, int line);
  static void LogSimple(const std::string message, bool new_line = true);
  static void SetLogFile(const std::string log_file);
  static void SetLogToStdOut(bool status);
 private:
  static std::unique_ptr<Logger> logger_;
  bool log_to_stdout_;
  std::string log_file_;
  std::ofstream ofs_;
  Logger() : log_to_stdout_(true) { }
  void LogMessage(const std::string message, bool new_line, const std::string prefix = "");
};

} // namespace hsisomap_logger

#endif //HSISOMAP_LOGGER_H
