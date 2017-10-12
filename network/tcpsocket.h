
#ifndef PROTEI_NETWORK_TCPSOCKET_H
#define PROTEI_NETWORK_TCPSOCKET_H

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
    TcpSocket(handle_t handle, bool connected = true) noexcept;
    virtual ~TcpSocket();

    void connect(const InternetAddress& address);
    void disconnect();

    bool connected() const noexcept;

    ssize_t write(const void* data, size_t size, size_t& writed);
    ssize_t read(void* data, size_t size, size_t& readed);

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

#endif // PROTEI_NETWORK_TCPSOCKET_H
