
#ifndef PROTEI_NETWORK_UDPSOCKET_H
#define PROTEI_NETWORK_UDPSOCKET_H

#include "socket.h"

namespace protei
{

class InternetAddress;
class UdpSocket : public Socket
{
public:

    UdpSocket();
    UdpSocket(const UdpSocket& copy) noexcept;
    UdpSocket(const InternetAddress& address);
    UdpSocket(handle_t handle, bool binded = false) noexcept;
    virtual ~UdpSocket() = default;

public:

    bool binded() const noexcept;
    void bind(const InternetAddress& address);

    void connect(const InternetAddress& address);

    ssize_t write(const void* data, size_t size, size_t& writed, const InternetAddress& address);
    ssize_t read(void* data, size_t size, size_t& readed, InternetAddress& address);

    inline void close()
    {
        Socket::close();
        m_binded = false;
    }

    inline void setReuseAddress(bool enabled)
    {
        Socket::setReuseAddress(enabled);
    }

    inline void setKeepAlive(bool enabled)
    {
        Socket::setKeepAlive(enabled);
    }

    inline size_t sendBufferSize() const
    {
        return Socket::sendBufferSize();
    }

    inline void setSendBufferSize(size_t size)
    {
        Socket::setSendBufferSize(size);
    }

    inline size_t receiveBufferSize() const
    {
        return Socket::receiveBufferSize();
    }

    inline void setReceiveBufferSize(size_t size)
    {
        Socket::setReceiveBufferSize(size);
    }

private:

    bool m_binded;
};

} // namespace protei

#endif // PROTEI_NETWORK_UDPSOCKET_H
