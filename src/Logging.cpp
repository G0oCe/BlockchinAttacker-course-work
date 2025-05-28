#include "Logging.hpp"
#include "console_colors.hpp" // Предполагаем, что этот файл существует
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

// Определения глобальных переменных
std::mutex g_log_mutex;
std::mutex g_results_mutex;
std::ofstream g_log_file;
std::ofstream g_results_file;

void init_logging(const std::string& log_path, const std::string& results_path) {
    g_log_file.open(log_path, std::ios_base::app);
    g_results_file.open(results_path, std::ios_base::app);
}

void close_logging() {
    if (g_log_file.is_open()) g_log_file.close();
    if (g_results_file.is_open()) g_results_file.close();
}

void safe_log(const std::string& message, const std::string& color_code) {
    std::lock_guard<std::mutex> guard(g_log_mutex);
    auto now_sys = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now_sys);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now_sys.time_since_epoch()) % 1000;
    std::stringstream ss_timestamp;
#if defined(_MSC_VER)
    struct tm buf; localtime_s(&buf, &in_time_t); ss_timestamp << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
#else
    ss_timestamp << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
#endif
    ss_timestamp << '.' << std::setfill('0') << std::setw(3) << ms.count();
    std::cout << color_code << "[" << ss_timestamp.str() << "] " << message << Color::RESET << std::endl;
    if (g_log_file.is_open()) { g_log_file << "[" << ss_timestamp.str() << "] " << message << std::endl; }
}

void log_result(const std::string& message_body) {
    std::lock_guard<std::mutex> guard(g_results_mutex);
    auto now_sys = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now_sys);
    std::stringstream ss_timestamp;
#if defined(_MSC_VER)
    struct tm buf; localtime_s(&buf, &in_time_t); ss_timestamp << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
#else
    ss_timestamp << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
#endif
    std::string timestamp_str = "[" + ss_timestamp.str() + "] ";
    std::cout << Color::GREEN << Color::BOLD << "RESULT: " << timestamp_str << message_body << Color::RESET << std::endl;
    if (g_results_file.is_open()) { g_results_file << timestamp_str << message_body << std::endl; }
}