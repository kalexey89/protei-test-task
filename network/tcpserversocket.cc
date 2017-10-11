
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
    m_listening = false;

    if (m_handle == InvalidHandle)
        create();

    setReuseAddress(reuseAddress);

    if (::bind(m_handle, static_cast<const sockaddr*>(address), static_cast<socklen_t>(address)) == -1)
        throw NetworkException(errno);

    if (::listen(m_handle, backlog) == -1)
        throw NetworkException(errno);

    m_listening = true;
}


Socket::Handle TcpServerSocket::accept(InternetAddress& clientAddress)
{
    struct sockaddr_in addr = { 0 };
    socklen_t addrlen = sizeof(addr);
    const Handle clientHandle = ::accept(m_handle, reinterpret_cast<sockaddr*>(&addr), &addrlen);
    if (clientHandle == InvalidHandle)
        throw NetworkException(errno);

    clientAddress = addr;

    return clientHandle;
}

} // namespace protei
