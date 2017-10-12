#include "udpserver.h"

namespace  protei
{

void UdpServer::listen(const InternetAddress &address)
{
    if (m_socket.binded())
        return;

    m_socket.bind(address);
    while (m_socket.valid() && m_socket.binded() && waitForReadyRead())
        m_handler(m_socket);
}


void UdpServer::close()
{
    m_socket.close();
}


bool UdpServer::listening() const noexcept
{
    return m_socket.binded();
}


bool UdpServer::waitForReadyRead() const noexcept
{
    char c = 0;
    return ::recvfrom(static_cast<Socket::handle_t>(m_socket), &c, 1, MSG_PEEK, nullptr, nullptr) > 0;
}

}
