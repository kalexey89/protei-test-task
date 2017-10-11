
#ifndef PROTEI_SOCKET_H
#define PROTEI_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>

#include <string>

namespace protei
{

class InternetAddress;
class Socket
{
public:

    enum class Shutdown
    {
        Read = 0,   /* SHUT_RD */
        Write = 1,  /* SHUT_WR */
        Both = 2    /* SHUT_RDWR */
    };

    typedef int Handle;

public:

    virtual ~Socket();

public:

    operator Handle () const;

public:

    bool valid() const noexcept;

    bool blocking() const;
    void setBlocking(bool enabled);

    bool closeOnDelete() const noexcept;
    void setCloseOnDelete(bool enabled) noexcept;

public:

    static constexpr Handle InvalidHandle = -1;

protected:

    enum class Type
    {
        Tcp = SOCK_STREAM,
        Udp = SOCK_DGRAM
    };


protected:

    Socket() = delete;
    Socket(Type type) noexcept;

    Handle handle() const noexcept;

    void create();

    void shutdown(Shutdown how);
    void close();

    InternetAddress localAddress() const;
    InternetAddress remoteAddress() const;

    void setReuseAddress(bool enabled);
    void setKeepAlive(bool enabled);

    size_t sendBufferSize() const;
    void setSendBufferSize(size_t size);

    size_t receiveBufferSize() const;
    void setReceiveBufferSize(size_t size);

    void setSendTimeout(const timeval& timeout);
    void setReceiveTimeout(const timeval& timeout);

    bool tcpNoDelay() const;
    void setTcpNoDelay(bool enabled);

protected:

    Handle m_handle;
    Type m_type;
    bool m_closeOnDelete;
};

} // namespace protei

#endif // PROTEI_SOCKET_H
