
#ifndef PROTEI_TCPSERVERSOCKET_H
#define PROTEI_TCPSERVERSOCKET_H

#include "socket.h"

namespace protei
{

class InternetAddress;
class TcpSocket;
class TcpServerSocket : public Socket
{
public:

    TcpServerSocket();
    TcpServerSocket(const InternetAddress& address, bool reuseAddress = true, int backlog = SOMAXCONN);

public:

    bool listening() const noexcept;
    void listen(const InternetAddress& address, bool reuseAddress = true, int backlog = SOMAXCONN);

    Handle accept(InternetAddress& clientAddress);

    inline void close()
    {
        Socket::close();
        m_listening = false;
    }

    inline void setReuseAddress(bool enabled)
    {
        Socket::setReuseAddress(enabled);
    }

    inline bool tcpNoDelay() const
    {
        return Socket::tcpNoDelay();
    }

    inline void setTcpNoDelay(bool enabled)
    {
        Socket::setTcpNoDelay(enabled);
    }

protected:

    bool m_listening;
};

} // namespace protei

#endif // PROTEI_TCPSERVERSOCKET_H
