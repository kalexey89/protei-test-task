
#ifndef PROTEI_NETWORK_NETWORKEXCEPTION_H
#define PROTEI_NETWORK_NETWORKEXCEPTION_H

#include <errno.h>

#include <exception>
#include <string>

namespace protei
{

class NetworkException final : public std::exception
{
public:

    explicit NetworkException(int errnum = errno);
    NetworkException(const char* message, int errnum = errno);
    NetworkException(const std::string& message, int errnum = errno);
    ~NetworkException() = default;

public:

    const char* what() const noexcept override;

    int errnum() const noexcept;

private:

    std::string m_message;
    int m_errnum;
};

} // namespace protei

#endif // PROTEI_NETWORK_NETWORKEXCEPTION_H
