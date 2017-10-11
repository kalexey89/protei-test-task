
#include "tcpsocket.h"
#include "networkexception.h"
#include "internetaddress.h"

namespace protei
{

TcpSocket::TcpSocket() :
    Socket(Type::Tcp),
    m_connected(false)
{
    create();
}


TcpSocket::TcpSocket(const TcpSocket& copy) noexcept :
    Socket(Type::Tcp)
{
    m_type = copy.m_type;
    m_handle = copy.m_handle;
    m_closeOnDelete = copy.m_closeOnDelete;
    m_connected = copy.m_connected;
}


TcpSocket::TcpSocket(const InternetAddress &address) :
    Socket(Type::Tcp),
    m_connected(false)
{
    connect(address);
}


TcpSocket::TcpSocket(Handle handle, bool connected) noexcept :
    Socket(Type::Tcp),
    m_connected(connected)
{
    m_handle = handle;
}


bool TcpSocket::connected() const noexcept
{
    return m_connected;
}


void TcpSocket::connect(const InternetAddress &address)
{
    if (m_handle == InvalidHandle)
        create();

    if (::connect(m_handle, static_cast<const sockaddr*>(address), static_cast<socklen_t>(address)) == -1)
        throw NetworkException(errno);

    m_connected = true;
}


void TcpSocket::disconnect()
{
    if (m_connected)
    {
        shutdown(Shutdown::Both);
        close();

        m_connected = false;
    }
}


ssize_t TcpSocket::write(const void* data, size_t size, size_t& writed)
{
    if (data == nullptr || size == 0)
        throw NetworkException(EINVAL);

    if (!m_connected)
        throw NetworkException(ENOTCONN);

    ssize_t result = 0;
    for (writed = 0; writed < size; writed += result)
    {
        result = ::send(m_handle, static_cast<const char*>(data) + writed, size - writed, 0);
        // Call send again
        if (result < 0 && (errno == EINTR || errno == EWOULDBLOCK)) result = 0;
        // Error
        else if (result < 0) return -1;
        // EOF
        else { writed += result; break; }
    }

    return result;
}


ssize_t TcpSocket::read(void* data, size_t size, size_t& readed)
{
    if (data == nullptr || size == 0)
        throw NetworkException(EINVAL);

    if (!m_connected)
        throw NetworkException(ENOTCONN);

    ssize_t result = 0;
    for (readed = 0; readed < size; readed += result)
    {
        result = ::recv(m_handle, static_cast<char*>(data) + readed, size - readed, 0);
        // Call recv again
        if (result < 0 && (errno == EINTR || errno == EWOULDBLOCK)) result = 0;
        // Error
        else if (result < 0) return -1;
        // EOF
        else { readed += result; break; }
    }

    *(static_cast<char*>(data) + size) = '\0';

    return result;
}


InternetAddress TcpSocket::localAddress() const
{
    return Socket::localAddress();
}


InternetAddress TcpSocket::remoteAddress() const
{
    return Socket::remoteAddress();
}

} // namespace protei
