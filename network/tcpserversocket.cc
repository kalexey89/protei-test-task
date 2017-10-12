
#include "tcpserversocket.h"
#include "tcpsocket.h"
#include "networkexception.h"
#include "internetaddress.h"

namespace protei
{

TcpServerSocket::TcpServerSocket() :
    Socket(Type::Tcp),
    m_listening(false)
{
    create();
}


TcpServerSocket::TcpServerSocket(const InternetAddress& address, bool reuseAddress, int backlog) :
    Socket(Type::Tcp),
    m_listening(false)
{
    listen(address, backlog);
}


bool TcpServerSocket::listening() const noexcept
{
    return m_listening;
}


void TcpServerSocket::listen(const InternetAddress& address, bool reuseAddress, int backlog)
{
    if (m_listening)
        close();

    m_listening = true;

    if (m_handle == InvalidHandle)
        create();

    setReuseAddress(reuseAddress);

    if (::bind(m_handle, static_cast<const sockaddr*>(address), static_cast<socklen_t>(address)) == -1)
        throw NetworkException(errno);

    if (::listen(m_handle, backlog) == -1)
        throw NetworkException(errno);

    m_listening = true;
}


Socket::handle_t TcpServerSocket::accept()
{
    return ::accept(m_handle, nullptr, nullptr);
}

} // namespace protei
