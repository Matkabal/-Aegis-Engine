#pragma once

#include <fstream>
#include <mutex>
#include <string>
#include <string_view>

namespace engine::core {

enum class LogLevel {
  Trace,
  Debug,
  Info,
  Warn,
  Error,
};

class Logger {
public:
  explicit Logger(std::string file_path = "logs/engine.log");
  ~Logger();

  void log(LogLevel level, std::string_view message);
  void trace(std::string_view message);
  void debug(std::string_view message);
  void info(std::string_view message);
  void warn(std::string_view message);
  void error(std::string_view message);

private:
  std::string timestamp_now() const;
  static const char* level_text(LogLevel level);

  std::mutex mutex_;
  std::ofstream file_;
};

} // namespace engine::core
