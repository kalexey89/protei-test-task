
#ifndef PROTEI_UDPSERVER_H
#define PROTEI_UDPSERVER_H

#include "udpsocket.h"

#include <functional>

namespace protei
{

class InternetAddress;
class UdpServer
{
public:

    typedef std::function<void(UdpSocket&)> handler_t;

public:

    UdpServer() = delete;
    inline explicit UdpServer(const handler_t& handler) : m_handler(handler) { ; }
    virtual ~UdpServer() = default;

    void listen(const InternetAddress& address);
    void close();

    bool listening() const noexcept;

private:

    bool waitForReadyRead() const noexcept;

private:

    UdpSocket m_socket;
    handler_t m_handler;
};

} // namespace protei

#endif // PROTEI_UDPSERVER_H
