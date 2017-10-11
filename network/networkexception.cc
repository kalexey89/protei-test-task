
#include "networkexception.h"

#include <cstring>

namespace protei
{

NetworkException::NetworkException(int errnum) :
    m_message(::strerror(errnum)),
    m_errnum(errnum)
{ ; }


NetworkException::NetworkException(const char *message, int errnum) :
    m_message(message == nullptr ? "" : message),
    m_errnum(errnum)
{ ; }


NetworkException::NetworkException(const std::string &message, int errnum) :
    m_message(message),
    m_errnum(errnum)
{ ; }


const char* NetworkException::what() const noexcept
{
    return m_message.c_str();
}


int NetworkException::errnum() const
{
    return m_errnum;
}

} // namespace protei
