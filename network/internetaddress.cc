
#include "internetaddress.h"
#include "networkexception.h"

#include <arpa/inet.h>

#include <cstring>
#include <array>

namespace protei
{

InternetAddress::InternetAddress() noexcept :
    m_valid(false)
{
    std::memset(&m_address, 0, sizeof(m_address));
    m_address.sin_family = AF_INET;
}


InternetAddress::InternetAddress(const InternetAddress& copy) noexcept
{
    m_address = copy.m_address;
    m_valid = copy.m_valid;
}


InternetAddress::InternetAddress(const char* hostname, port_t port) :
    m_valid(false)
{
    std::memset(&m_address, 0, sizeof(m_address));
    m_address.sin_family = AF_INET;
    m_address.sin_port = ::htons(port);

    setHost(hostname);
}


InternetAddress::InternetAddress(const std::string& hostname, port_t port) :
    InternetAddress(hostname.c_str(), port)
{ ; }


InternetAddress::InternetAddress(InternetAddress::host_t host, port_t port) noexcept :
    m_valid(true)
{
    std::memset(&m_address, 0, sizeof(m_address));
    m_address.sin_family = AF_INET;
    m_address.sin_port = ::htons(port);
    m_address.sin_addr.s_addr = ::htonl(host);
}


InternetAddress::InternetAddress(SpecialIpAddress host, port_t port) noexcept :
    InternetAddress(static_cast<host_t>(host), port)
{ ; }


InternetAddress::InternetAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d, port_t port) noexcept :
    InternetAddress((a << 24 | b << 16 | c << 8 | d), port)
{ ; }


InternetAddress::InternetAddress(const InternetAddress::address_t& address)
{
    if (address.sin_family != AF_INET)
        throw NetworkException(EAFNOSUPPORT);

    m_address = address;
}


InternetAddress& InternetAddress::operator = (const InternetAddress& rhs) noexcept
{
    m_address = rhs.m_address;
    m_valid = rhs.m_valid;

    return *this;
}


InternetAddress& InternetAddress::operator = (const address_t& rhs)
{
    if (rhs.sin_family != AF_INET)
        throw NetworkException(EAFNOSUPPORT);

    m_address = rhs;
    m_valid = true;

    return *this;
}


InternetAddress::operator const sockaddr* () const noexcept
{
    return reinterpret_cast<const sockaddr*>(&m_address);
}


InternetAddress::operator const address_t* () const noexcept
{
    return reinterpret_cast<const address_t*>(&m_address);
}


InternetAddress::operator length_t() const noexcept
{
    return sizeof(m_address);
}


InternetAddress::operator std::string () const noexcept
{
    return toString();
}


bool InternetAddress::valid() const noexcept
{
    return m_valid;
}


InternetAddress::host_t InternetAddress::host() const noexcept
{
    return ::ntohl(m_address.sin_addr.s_addr);
}


void InternetAddress::setHost(const char *hostname)
{
    m_address.sin_addr.s_addr = ::htonl(INADDR_NONE);
    m_valid = false;

    if (hostname == nullptr || std::strlen(hostname) == 0)
        return;

    struct addrinfo hints = { 0 };
    hints.ai_family = AF_INET;
    hints.ai_socktype = 0;
    hints.ai_protocol = 0;

    struct addrinfo* addrlist = nullptr;
    const int result = ::getaddrinfo(hostname, nullptr, &hints, &addrlist);
    if (result != 0 || addrlist == nullptr)
    {
        if (addrlist)
            ::freeaddrinfo(addrlist);

        throw NetworkException(errno);
    }

    m_address.sin_addr = reinterpret_cast<address_t*>(addrlist->ai_addr)->sin_addr;
    m_valid = true;

    ::freeaddrinfo(addrlist);
}


void InternetAddress::setHost(const std::string &hostname)
{
    setHost(hostname.c_str());
}


void InternetAddress::setHost(host_t host) noexcept
{
    m_address.sin_addr.s_addr = ::htonl(host);
}


void InternetAddress::setHost(uint8_t a, uint8_t b, uint8_t c, uint8_t d) noexcept
{
    setHost((a << 24 | b << 16 | c << 8 | d));
}


InternetAddress::port_t InternetAddress::port() const noexcept
{
    return ::ntohs(m_address.sin_port);
}


void InternetAddress::setPort(port_t port) noexcept
{
    m_address.sin_port = ::htons(port);
}


InternetAddress::address_t InternetAddress::address() const noexcept
{
    return m_address;
}


void InternetAddress::setAddress(const address_t &address)
{
    if (address.sin_family != AF_INET)
        throw NetworkException(EAFNOSUPPORT);

    m_address = address;
}


std::string InternetAddress::toString() const noexcept
{
    std::array<char, INET_ADDRSTRLEN> buffer = { '\0' };
    if (::inet_ntop(AF_INET, &m_address.sin_addr, buffer.data(), buffer.size()) == nullptr)
    {
        return std::string("0.0.0.0:0");
    }

    std::array<char, INET_ADDRSTRLEN + 1 + 5 + 1> addr = { '\0' }; // ip + ':' + port + '\0'
    std::snprintf(addr.data(), addr.size(), "%s:%u", buffer.data(), port());

    return addr.data();
}

} // namespace protei
