//
// Created by Can on 1/12/16.
//

#include <hsisomap/Logger.h>
#include <iostream>
#include <ctime>

namespace hsisomap_logger {

std::unique_ptr<Logger> Logger::logger_ = nullptr;

Logger::~Logger() {
  if (logger_.get()) {
    if (logger_->ofs_.is_open()) ofs_.close();
  }
}

void Logger::Log(const std::string message,
                 bool time_stamp,
                 bool local_time,
                 std::string time_format,
                 bool show_source_location,
                 bool new_line,
                 const std::string tag,
                 const std::string prefix,
                 char const *function,
                 char const *file,
                 int line) {
  if (!logger_.get()) logger_.reset(new Logger);

  if (time_format.empty()) time_format = "[%X]";
  std::time_t now = time(NULL);
  std::tm tstruct;
  char buf[80];
  tstruct = local_time ? *localtime(&now) : *gmtime(&now);
  std::strftime(buf, sizeof(buf), time_format.c_str(), &tstruct);

  std::stringstream sstr;
  sstr << (time_stamp ? buf : "") << tag << " " << message;
  if (show_source_location) {
    sstr << " LINE: " << line << " FUNCTION: " << function << " FILE: " << file;
  }
  logger_->LogMessage(sstr.str(), new_line, prefix);
}

void Logger::LogSimple(const std::string message, bool new_line) {
  if (!logger_.get()) logger_.reset(new Logger);
  logger_->LogMessage(message, new_line);
}

void Logger::SetLogFile(const std::string log_file) {
  if (!logger_.get()) logger_.reset(new Logger);
  if (log_file.empty()) {
    if (logger_->ofs_.is_open()) logger_->ofs_.close();
    logger_->log_file_ = log_file;
  }
  if (log_file != logger_->log_file_) {
    if (logger_->ofs_.is_open()) logger_->ofs_.close();
    logger_->log_file_ = log_file;
    logger_->ofs_.open(log_file, std::ios::out | std::ios::app);
  }
}

void Logger::SetLogToStdOut(bool status) {
  if (!logger_.get()) logger_.reset(new Logger);
  logger_->log_to_stdout_ = status;
}

void Logger::LogMessage(const std::string message, bool new_line, const std::string prefix) {
  if (log_to_stdout_) {
#ifndef NLOGTERMCOLOR
    std::cout << prefix;
#endif
    std::cout << message;
#ifndef NLOGTERMCOLOR
    if (prefix[0] == '\033') std::cout << "\033[0m"; // If it has terminal control, restore status
#endif
    if (new_line) std::cout << std::endl;
  }
  if (logger_->ofs_.is_open()) {
#ifdef LOGFILECOLOR
    logger_->ofs_ << prefix;
#endif
    logger_->ofs_ << message;
#ifdef LOGFILECOLOR
    if (prefix[0] == '\033') logger_->ofs_ << "\033[0m"; // If it has terminal control, restore status
#endif
    if (new_line) logger_->ofs_ << std::endl;
  }
}

} // namespace hsisomap_logger