
#include "udpsocket.h"
#include "networkexception.h"
#include "internetaddress.h"

namespace protei
{

UdpSocket::UdpSocket() :
    Socket(Type::Udp),
    m_binded(false)
{
    create();
}


UdpSocket::UdpSocket(const UdpSocket& copy) noexcept :
    Socket(Type::Udp)
{
    m_type = copy.m_type;
    m_handle = copy.m_handle;
    m_binded = copy.m_binded;
}


UdpSocket::UdpSocket(const InternetAddress& address) :
    Socket(Type::Udp),
    m_binded(false)
{
    bind(address);
}


UdpSocket::UdpSocket(handle_t handle, bool binded) noexcept :
    Socket(Type::Udp),
    m_binded(binded)
{
    m_handle = handle;
}


bool UdpSocket::binded() const noexcept
{
    return m_binded;
}


void UdpSocket::bind(const InternetAddress &address)
{
    if (m_handle == InvalidHandle)
        create();

    if (::bind(m_handle, static_cast<const sockaddr*>(address), static_cast<socklen_t>(address)) == -1)
        throw NetworkException(errno);

    m_binded = true;
}


ssize_t UdpSocket::write(const void* data, size_t size, size_t& writed, const InternetAddress& address)
{
    if (data == nullptr || size == 0)
        throw NetworkException(EINVAL);

    ssize_t result = 0;
    for (writed = 0; writed < size; writed += result)
    {
        result = ::sendto(m_handle, static_cast<const char*>(data) + writed, size - writed, 0,
                          static_cast<const sockaddr*>(address), static_cast<socklen_t>(address));
        // Call sendto again
        if (result < 0 && (errno == EINTR || errno == EWOULDBLOCK)) result = 0;
        // Error
        else if (result < 0) return -1;
        // EOF
        else { writed += result; break; }
    }

    return result;
}


ssize_t UdpSocket::read(void* data, size_t size, size_t& readed, InternetAddress& address)
{
    if (data == nullptr || size == 0)
        throw NetworkException(EINVAL);

    ssize_t result = 0;
    for (readed = 0; readed < size; readed += result)
    {
        struct sockaddr_in addr = { 0 };
        socklen_t addrlen = sizeof(addr);
        result = ::recvfrom(m_handle, static_cast<char*>(data) + readed, size - readed, 0,
                            reinterpret_cast<sockaddr*>(&addr), &addrlen);
        address = addr;
        // Call sendto again
        if (result < 0 && (errno == EINTR || errno == EWOULDBLOCK)) result = 0;
        // Error
        else if (result < 0) return -1;
        // EOF
        else { readed += result; break; }
    }

    *(static_cast<char*>(data) +  size) = '\0';

    return result;
}

} // namespace protei
