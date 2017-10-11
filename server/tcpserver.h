
#ifndef PROTEI_TCPSERVER_H
#define PROTEI_TCPSERVER_H

#include "tcpserversocket.h"

#include <functional>
#include <atomic>
#include <thread>

namespace protei
{

class InternetAddress;
class TcpServer final
{
public:

    typedef std::function<void (Socket::Handle clientHandle)> request_handler_t;

public:

    TcpServer();
    TcpServer(const request_handler_t& handler);

    bool started() const noexcept;
    void start(const InternetAddress& address);
    void startDetached(const InternetAddress& address);
    void stop();

    void setRequestHandler(const request_handler_t& handler);

private:

    static constexpr size_t backlog = 16;

    TcpServerSocket m_serverSocket;
    request_handler_t m_requestHandler;
    std::atomic_bool m_started;
    std::thread m_thread;
};

} // namespace protei

#endif // PROTEI_TCPSERVER_H
