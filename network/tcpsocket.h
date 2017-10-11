
#ifndef PROTEI_TCPSOCKET_H
#define PROTEI_TCPSOCKET_H

#include "socket.h"

namespace protei
{

class InternetAddress;
class TcpSocket : public Socket
{
public:

    TcpSocket();
    TcpSocket(const TcpSocket& copy) noexcept;
    TcpSocket(const InternetAddress& address);
    TcpSocket(Handle handle, bool connected = true) noexcept;
    virtual ~TcpSocket() = default;

    bool connected() const noexcept;
    void connect(const InternetAddress& address);
    void disconnect();

    ssize_t write(const void* data, size_t size, size_t& writed);
    ssize_t read(void* data, size_t size, size_t& readed);

    InternetAddress localAddress() const;
    InternetAddress remoteAddress() const;

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

    inline void setSendTimeout(const timeval& timeout)
    {
        Socket::setSendTimeout(timeout);
    }

    inline void setReceiveTimeout(const timeval& timeout)
    {
        Socket::setReceiveTimeout(timeout);
    }

    inline bool tcpNoDelay() const
    {
        return Socket::tcpNoDelay();
    }

    inline void setTcpNoDelay(bool enabled)
    {
        Socket::setTcpNoDelay(enabled);
    }

private:

    bool m_connected;
};

} // namespace protei

#endif // PROTEI_TCPSOCKET_H
