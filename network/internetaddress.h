
#ifndef PROTEI_SOCKETADDRESS_H
#define PROTEI_SOCKETADDRESS_H

#include <netdb.h>

#include <cstdint>
#include <string>

namespace protei
{

enum class SpecialIpAddress : in_addr_t
{
    None        = INADDR_NONE,
    Any         = INADDR_ANY,
    Loopback    = INADDR_LOOPBACK,
    Broadcast   = INADDR_BROADCAST
};

class InternetAddress
{
public:

    typedef in_addr_t host_t;
    typedef in_port_t port_t;
    typedef socklen_t length_t;
    typedef struct sockaddr_in address_t;

public:

    InternetAddress() noexcept;
    InternetAddress(const InternetAddress& copy) noexcept;
    InternetAddress(const char* hostname, port_t port);
    InternetAddress(const std::string& hostname, port_t port);
    InternetAddress(host_t host, port_t port) noexcept;
    InternetAddress(SpecialIpAddress host, port_t port) noexcept;
    InternetAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d, port_t port) noexcept;
    InternetAddress(const address_t& address);
    virtual ~InternetAddress() = default;

public:

    InternetAddress& operator = (const InternetAddress& rhs) noexcept;
    InternetAddress& operator = (const address_t& rhs);

    operator const sockaddr* () const noexcept;
    operator const address_t* () const noexcept;

    operator length_t () const noexcept;

    operator std::string () const noexcept;

public:

    bool valid() const noexcept;

    host_t host() const noexcept;
    void setHost(const char* hostname);
    void setHost(const std::string& hostname);
    void setHost(host_t host) noexcept;
    void setHost(uint8_t a, uint8_t b, uint8_t c, uint8_t d) noexcept;

    port_t port() const noexcept;
    void setPort(port_t port) noexcept;

    address_t address() const noexcept;
    void setAddress(const address_t& address);

    std::string toString() const noexcept;

private:

    address_t m_address;
    bool m_valid;
};

inline std::ostream& operator << (std::ostream& cout, const InternetAddress& addr) noexcept
{
    return cout << addr.toString();
}

} // namespace protei

#endif // PROTEI_SOCKETADDRESS_H
