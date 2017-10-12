
#include "tcpserver.h"
#include "networkexception.h"
#include "internetaddress.h"

#include <thread>

namespace protei
{

void TcpServer::listen(const InternetAddress& address)
{
    if (m_socket.listening())
        return;

    m_socket.setTcpNoDelay(true);
    m_socket.listen(address, true, TcpServer::Backlog);

    while (m_socket.listening())
        std::thread(std::bind(m_handler, m_socket.accept())).detach();
}


void TcpServer::close()
{
    m_socket.close();
}


bool TcpServer::listening() const noexcept
{
    return m_socket.listening();
}

} // namespace protei
