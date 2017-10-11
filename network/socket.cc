
#include "socket.h"
#include "networkexception.h"
#include "internetaddress.h"

#include <netinet/tcp.h>

#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

namespace protei
{

Socket::Socket(Type type) noexcept :
    m_handle(InvalidHandle),
    m_type(type),
    m_closeOnDelete(true)
{ ; }


Socket::~Socket()
{
    if (m_handle != InvalidHandle && m_closeOnDelete)
    {
        ::shutdown(m_handle, static_cast<int>(Shutdown::Both));
        ::close(m_handle);
    }
}


Socket::operator Handle () const
{
    return m_handle;
}


bool Socket::valid() const noexcept
{
    return m_handle != InvalidHandle;
}


bool Socket::blocking() const
{
    bool value = false;
#ifdef O_NONBLOCK
    const int flags = ::fcntl(m_handle, F_GETFL, 0);
    value = ((flags > 0) && ((flags & O_NONBLOCK) == 0));
#elif defined(SO_NONBLOCK)
    int optval = 0;
    socklen_t optlen = sizeof(value);
    if (::getsockopt(m_handle, SOL_SOCKET, SO_NONBLOCK, &optval, &optlen) == -1)
    {
        throw NetworkException(errno);
    }

    value = optval == 0 ? true : false;
#endif
    return value;
}


void Socket::setBlocking(bool enabled)
{
#ifdef O_NONBLOCK
    int flags = ::fcntl(m_handle, F_GETFL, 0);
    if (flags < 0)
    {
        throw NetworkException(errno);
    }

    flags = enabled ? flags &=(~O_NONBLOCK) : flags |= O_NONBLOCK;
    if (::fcntl(m_handle, F_SETFL, flags) == -1)
    {
        throw NetworkException(errno);
    }
#elif defined(SO_NONBLOCK)
    int optval = enabled ? 0 : 1;
    if (::setsockopt(m_handle, SOL_SOCKET, SO_NONBLOCK, &optval, sizeof(optval)) == -1)
    {
        throw NetworkException(errno);
    }
#endif
}


bool Socket::closeOnDelete() const noexcept
{
    return m_closeOnDelete;
}


void Socket::setCloseOnDelete(bool enabled) noexcept
{
    m_closeOnDelete = enabled;
}


Socket::Handle Socket::handle() const noexcept
{
    return m_handle;
}


void Socket::create()
{
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
    if (::shutdown(m_handle, static_cast<int>(how)) == -1)
        throw NetworkException(errno);
}


void Socket::close()
{
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


void Socket::setSendTimeout(const timeval& timeout)
{
#ifdef SO_SNDTIMEO
    if (::setsockopt(m_handle, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1)
        throw NetworkException(errno);
#endif
}


void Socket::setReceiveTimeout(const timeval& timeout)
{
#ifdef SO_RCVTIMEO
    if (::setsockopt(m_handle, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1)
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
