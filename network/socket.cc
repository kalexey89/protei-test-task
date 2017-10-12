
#include "socket.h"
#include "networkexception.h"
#include "internetaddress.h"

#include <netinet/tcp.h>

#include <unistd.h>
#include <fcntl.h>

namespace protei
{

Socket::Socket(Type type) noexcept :
    m_handle(InvalidHandle),
    m_type(type)
{ ; }


Socket::~Socket()
{
    if (m_handle != InvalidHandle)
    {
        ::shutdown(m_handle, static_cast<int>(Shutdown::Both));
        ::close(m_handle);
    }
}


Socket::operator handle_t () const noexcept
{
    return m_handle;
}


bool Socket::valid() const noexcept
{
    return m_handle != InvalidHandle;
}


Socket::handle_t Socket::handle() const noexcept
{
    return m_handle;
}


void Socket::create()
{
    if (m_handle != InvalidHandle)
    {
        shutdown(Shutdown::Both);
        close();
    }

    switch (m_type)
    {
        case Type::Tcp: m_handle = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); break;
        case Type::Udp: m_handle = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); break;
        default: break;
    }

    if (m_handle == InvalidHandle)
        throw NetworkException(errno);
}


void Socket::shutdown(Shutdown how)
{
    if (m_handle == InvalidHandle)
        return;

    if (::shutdown(m_handle, static_cast<int>(how)) == -1)
        throw NetworkException(errno);
}


void Socket::close()
{
    if (m_handle == InvalidHandle)
        return;

    if (::close(m_handle) == -1)
        throw NetworkException(errno);

    m_handle = InvalidHandle;
}


InternetAddress Socket::localAddress() const
{
    struct sockaddr_in addr = { 0 };
    if (m_handle != InvalidHandle)
    {
        socklen_t addrlen = sizeof(addr);
        if (::getsockname(m_handle, reinterpret_cast<sockaddr*>(&addr), &addrlen) == -1)
            throw NetworkException(errno);
    }

    return addr;
}


InternetAddress Socket::remoteAddress() const
{
    struct sockaddr_in addr = { 0 };
    if (m_handle != InvalidHandle)
    {
        socklen_t addrlen = sizeof(addr);
        if (::getpeername(m_handle, reinterpret_cast<sockaddr*>(&addr), &addrlen) == -1)
            throw NetworkException(errno);
    }

    return addr;
}


void Socket::setReuseAddress(bool enabled)
{
    int optval = enabled ? 1 : 0;
#ifdef SO_REUSEADDR
    if (::setsockopt(m_handle, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        throw NetworkException(errno);
#endif
}


void Socket::setKeepAlive(bool enabled)
{
    int optval = enabled ? 1 : 0;
#ifdef SO_KEEPALIVE
    if (::setsockopt(m_handle, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) == -1)
    {
        throw NetworkException(errno);
    }
#endif
}


size_t Socket::sendBufferSize() const
{
    size_t optval = 0;
#ifdef SO_SNDBUF
    socklen_t optlen = sizeof(optval);
    if (::getsockopt(m_handle, SOL_SOCKET, SO_SNDBUF, &optval, &optlen) == -1)
        throw NetworkException(errno);
#endif
    return optval;
}


void Socket::setSendBufferSize(size_t size)
{
#ifdef SO_SNDBUF
    if (::setsockopt(m_handle, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) == -1)
        throw NetworkException(errno);
#endif
}


size_t Socket::receiveBufferSize() const
{
    size_t optval = 0;
#ifdef SO_RCVBUF
    socklen_t optlen = sizeof(optval);
    if (::getsockopt(m_handle, SOL_SOCKET, SO_RCVBUF, &optval, &optlen) == -1)
        throw NetworkException(errno);
#endif
    return optval;
}


void Socket::setReceiveBufferSize(size_t size)
{
#ifdef SO_RCVBUF
    if (::setsockopt(m_handle, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) == -1)
        throw NetworkException(errno);
#endif
}


bool Socket::tcpNoDelay() const
{
    int optval = 0;
#ifdef TCP_NODELAY
    socklen_t optlen = sizeof(optval);
    if (::getsockopt(m_handle, IPPROTO_TCP, TCP_NODELAY, &optval, &optlen) == -1)
        throw NetworkException(errno);
#endif
    return optval == 1 ? true : false;
}


void Socket::setTcpNoDelay(bool enabled)
{
    int optval = enabled ? 1 : 0;
#ifdef TCP_NODELAY
    if (::setsockopt(m_handle, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval)) == -1)
        throw NetworkException(errno);
#endif
}

} // namespace protei
