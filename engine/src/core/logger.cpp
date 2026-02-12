#include "engine/core/logger.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

#ifdef ENGINE_HAS_FMT
#include <fmt/format.h>
#endif

namespace engine::core {

Logger::Logger(std::string file_path) {
  std::filesystem::path path(std::move(file_path));
  if (path.has_parent_path()) {
    std::filesystem::create_directories(path.parent_path());
  }
  file_.open(path, std::ios::out | std::ios::app);
}

Logger::~Logger() {
  if (file_.is_open()) {
    file_.flush();
  }
}

void Logger::log(LogLevel level, std::string_view message) {
  std::lock_guard<std::mutex> lock(mutex_);
  std::ostringstream tid_stream;
  tid_stream << std::this_thread::get_id();
  const std::string tid_text = tid_stream.str();

#ifdef ENGINE_HAS_FMT
  const std::string line = fmt::format("[{}][{}][tid={}] {}",
                                       timestamp_now(),
                                       level_text(level),
                                       tid_text,
                                       message);
#else
  std::ostringstream line;
  line << '[' << timestamp_now() << ']'
       << '[' << level_text(level) << ']'
       << "[tid=" << tid_text << "] "
       << message;
#endif

  std::cout
#ifdef ENGINE_HAS_FMT
      << line
#else
      << line.str()
#endif
      << '\n';
  if (file_.is_open()) {
    file_
#ifdef ENGINE_HAS_FMT
        << line
#else
        << line.str()
#endif
        << '\n';
    file_.flush();
  }
}

void Logger::trace(std::string_view message) { log(LogLevel::Trace, message); }
void Logger::debug(std::string_view message) { log(LogLevel::Debug, message); }
void Logger::info(std::string_view message) { log(LogLevel::Info, message); }
void Logger::warn(std::string_view message) { log(LogLevel::Warn, message); }
void Logger::error(std::string_view message) { log(LogLevel::Error, message); }

std::string Logger::timestamp_now() const {
  const auto now = std::chrono::system_clock::now();
  const std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  std::tm local_tm{};
#if defined(_WIN32)
  localtime_s(&local_tm, &now_c);
#else
  localtime_r(&now_c, &local_tm);
#endif

  std::ostringstream out;
  out << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");
  return out.str();
}

const char* Logger::level_text(LogLevel level) {
  switch (level) {
  case LogLevel::Trace:
    return "TRACE";
  case LogLevel::Debug:
    return "DEBUG";
  case LogLevel::Info:
    return "INFO";
  case LogLevel::Warn:
    return "WARN";
  case LogLevel::Error:
    return "ERROR";
  default:
    return "UNKNOWN";
  }
}

} // namespace engine::core
