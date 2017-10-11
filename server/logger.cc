
#include "logger.h"

#include <cstdarg>
#include <cstring>

namespace  protei
{

void Logger::operator () (const char* format, ...)
{
    std::lock_guard<std::mutex> locker(m_mutex);

    std::va_list args;
    va_start(args, format);
    std::vprintf(format, args);
    va_end(args);
}

void Logger::operator () (const std::string& format, ...)
{
    operator () (format.c_str());
}


} // namespace protei
