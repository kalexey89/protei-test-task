
#ifndef PROTEI_SERVER_LOGGER_H
#define PROTEI_SERVER_LOGGER_H

#include <mutex>
#include <string>

namespace protei
{

static class Logger final
{
public:

    Logger() = default;
    ~Logger() = default;

public:

    void operator () (const char* format, ...) ;
    void operator () (const std::string& format, ...);

private:

    std::mutex m_mutex;
} logger;

} // namespace protei

#endif // PROTEI_SERVER_LOGGER_H
