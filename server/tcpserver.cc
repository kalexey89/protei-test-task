
#include "tcpserver.h"
#include "internetaddress.h"

#include <thread>

namespace protei
{


TcpServer::TcpServer() :
    m_requestHandler(nullptr),
    m_started(false)
{ ; }


TcpServer::TcpServer(const request_handler_t& handler) :
    m_requestHandler(handler),
    m_started(false)
{ ; }


bool TcpServer::started() const noexcept
{
    return m_started;
}


void TcpServer::start(const InternetAddress& address)
{
    m_serverSocket.setTcpNoDelay(true);
    m_serverSocket.listen(address, true, TcpServer::backlog);

    m_started = true;

    InternetAddress clientAddress = {};
    Socket::Handle clientHandle = Socket::InvalidHandle;
    while (m_started)
    {
        clientHandle = m_serverSocket.accept(clientAddress);
        std::thread(std::bind(m_requestHandler, clientHandle)).detach();
    }
}


void TcpServer::stop()
{
    m_started = false;
}


void TcpServer::setRequestHandler(const request_handler_t& handler)
{
    m_requestHandler = handler;
}

} // namespace protei
