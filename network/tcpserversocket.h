
#ifndef PROTEI_NETWORK_TCPSERVERSOCKET_H
#define PROTEI_NETWORK_TCPSERVERSOCKET_H

#include "socket.h"

namespace protei
{

class InternetAddress;
class TcpServerSocket : public Socket
{
public:

    TcpServerSocket();
    TcpServerSocket(const InternetAddress& address, bool reuseAddress = true, int backlog = SOMAXCONN);

public:

    void listen(const InternetAddress& address, bool reuseAddress = true, int backlog = SOMAXCONN);
    inline void close()
    {
        Socket::close();
        m_listening = false;
    }

    bool listening() const noexcept;

    handle_t accept();

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

#endif // PROTEI_NETWORK_TCPSERVERSOCKET_H
