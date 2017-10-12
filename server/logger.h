
#ifndef PROTEI_SERVER_LOGGER_H
#define PROTEI_SERVER_LOGGER_H

#include <mutex>
#include <sstream>

#define logger Logger {}

namespace protei
{

class Logger final : public std::ostringstream
{
public:

    Logger() = default;
    ~Logger();

private:

    static std::mutex m_mutex;
};

} // namespace protei

#endif // PROTEI_SERVER_LOGGER_H
