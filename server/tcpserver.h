
#ifndef PROTEI_TCPSERVER_H
#define PROTEI_TCPSERVER_H

#include "tcpserversocket.h"
#include "tcpsocket.h"

#include <functional>

namespace protei
{

class InternetAddress;
class TcpServer
{
public:

    typedef std::function<void(Socket::handle_t)> handler_t;

public:

    TcpServer() = delete;
    inline explicit TcpServer(const handler_t& handler) : m_handler(handler) { ; }
    virtual ~TcpServer() = default;

    void listen(const InternetAddress& address);
    void close();

    bool listening() const noexcept;

private:

    static constexpr int Backlog = 64;

    TcpServerSocket m_socket;
    handler_t m_handler;
};

} // namespace protei

#endif // PROTEI_TCPSERVER_H
