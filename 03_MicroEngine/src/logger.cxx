#include "./../include/engine.hxx"
Logger::Logger(const std::string& log_file) {
  if (log_file.empty())
    log.open("Engine.log");
  else
    log.open(log_file);
}

Logger::~Logger() {
  log.close();
}

Logger& Logger::get_instance(const std::string& log_file) {
  static Logger instance(log_file);
  return instance;
}

void Logger::add_log(const std::string& log_string, log_level level) {
  if (!log)
    return;

  std::string prefix;
  switch (level) {
    case log_level::log_warning:
      prefix = "Warning: ";
    case log_level::log_error:
      prefix = "Error: ";
  }

  log << prefix << log_string << "." << std::endl;
}
