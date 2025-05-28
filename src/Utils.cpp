#include "Utils.hpp"
#include <sstream>
#include <iomanip>

std::string format_duration(std::chrono::milliseconds ms_total) {
    auto hrs = std::chrono::duration_cast<std::chrono::hours>(ms_total); ms_total -= hrs;
    auto mins = std::chrono::duration_cast<std::chrono::minutes>(ms_total); ms_total -= mins;
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(ms_total); ms_total -= secs;
    auto ms = ms_total;
    std::ostringstream oss; oss << std::setfill('0');
    if (hrs.count() > 0) oss << hrs.count() << "h ";
    if (mins.count() > 0 || hrs.count() > 0) oss << std::setw(hrs.count() > 0 ? 2 : 1) << mins.count() << "m ";
    if (secs.count() > 0 || mins.count() > 0 || hrs.count() > 0) oss << std::setw((mins.count() > 0 || hrs.count() > 0) ? 2 : 1) << secs.count() << "s ";
    oss << std::setw(3) << ms.count() << "ms";
    return oss.str();
}