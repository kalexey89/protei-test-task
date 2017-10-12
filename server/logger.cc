
#include "logger.h"

#include <iostream>

namespace  protei
{

std::mutex Logger::m_mutex = {};

Logger::~Logger()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    std::cout << this->str();
}

} // namespace protei
