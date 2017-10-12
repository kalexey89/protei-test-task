
#ifndef PROTEI_NETWORK_SOCKET_H
#define PROTEI_NETWORK_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>

#include <vector>
#include <string>

namespace protei
{

class InternetAddress;
class Socket
{
public:

    enum class Shutdown
    {
        Read    = 0,   /* SHUT_RD */
        Write   = 1,  /* SHUT_WR */
        Both    = 2    /* SHUT_RDWR */
    };

    typedef int handle_t;
    typedef struct pollfd pollfd_t;

public:

    virtual ~Socket();

public:

    operator handle_t () const noexcept;

public:

    bool valid() const noexcept;

    InternetAddress localAddress() const;
    InternetAddress remoteAddress() const;

public:

    static constexpr handle_t InvalidHandle = -1;

protected:

    enum class Type
    {
        Tcp = SOCK_STREAM,
        Udp = SOCK_DGRAM
    };

protected:

    Socket() = delete;
    Socket(Type type) noexcept;

    handle_t handle() const noexcept;

    void create();

    void shutdown(Shutdown how);
    void close();

    void setReuseAddress(bool enabled);
    void setKeepAlive(bool enabled);

    size_t sendBufferSize() const;
    void setSendBufferSize(size_t size);

    size_t receiveBufferSize() const;
    void setReceiveBufferSize(size_t size);

    bool tcpNoDelay() const;
    void setTcpNoDelay(bool enabled);

protected:

    handle_t m_handle;
    Type m_type;
};

} // namespace protei

#endif // PROTEI_NETWORK_SOCKET_H
