#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <string>
#include <mutex>
#include <fstream>

// Объявления глобальных переменных (определения будут в .cpp)
extern std::mutex g_log_mutex;
extern std::mutex g_results_mutex;
extern std::ofstream g_log_file;
extern std::ofstream g_results_file;

// Объявления функций логирования
void init_logging(const std::string& log_path, const std::string& results_path);
void close_logging();
void safe_log(const std::string& message, const std::string& color_code = "");
void log_result(const std::string& message_body);

#endif // LOGGING_HPP