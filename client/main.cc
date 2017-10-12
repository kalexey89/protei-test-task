
#include "networkexception.h"
#include "internetaddress.h"
#include "tcpsocket.h"
#include "udpsocket.h"

#include <getopt.h>

#include <cstring>
#include <iostream>

using namespace protei;

static constexpr size_t MaxMessageSize = 65507; // Max UDP message size

namespace
{

void initTcpClient(const std::string& host, InternetAddress::port_t port)
{
    TcpSocket socket;
    socket.connect(InternetAddress(host, port));

    size_t readed = 0;
    size_t writed = 0;

    std::string buffer(MaxMessageSize, '\0');

    std::cout << std::endl << "Client: ";
    while (std::cin.getline(&buffer[0], MaxMessageSize))
    {
        const size_t inputed = std::cin.gcount();
        if (inputed == 5 && buffer.substr(0, inputed - 1) == "quit")
            break;

        socket.write(buffer.data(), inputed, writed);
        if (socket.read(&buffer[0], inputed, readed) == 0)
        {
            std::cout << std::endl << "Connection closed" << std::endl;
            exit(EXIT_SUCCESS);
        }

        std::cout << "Server: " << buffer.substr(0, inputed) << std::endl;
        std::fill(buffer.begin(), buffer.begin() + inputed, '\0');
        std::cout << std::endl << "Client: ";
    }
}


void initUdpClient(const std::string& host, InternetAddress::port_t port)
{
    InternetAddress addr(host, port);
    InternetAddress tmp = {};

    UdpSocket socket;

    size_t readed = 0;
    size_t writed = 0;

    std::string buffer(MaxMessageSize, '\0');

    std::cout << std::endl << "Client: ";
    while (std::cin.getline(&buffer[0], MaxMessageSize))
    {
        const size_t inputed = std::cin.gcount();
        if (inputed == 5 && buffer.substr(0, inputed - 1) == "quit")
            break;

        socket.write(buffer.data(), inputed, writed, addr);
        std::cout << "read: " << socket.read(&buffer[0], inputed, readed, tmp) <<std::endl;
        std::cout << "Server: " << buffer.substr(0, inputed) << std::endl;

        std::fill(buffer.begin(), buffer.begin() + inputed, '\0');

        std::cout << std::endl << "Client: ";
    }
}

} // namespace

int main(int argc, char* argv[])
{
    const struct option options[] = {
        { "proto", required_argument, NULL, 'q' },
        { "port", required_argument, NULL, 'p' },
        { "addr", optional_argument, NULL, 'a' },
        { NULL, no_argument, NULL, 0 }
    };

    std::string proto = {};
    std::string address = "127.0.0.1";
    InternetAddress::port_t port = 0;

    const char* optstr = "q:p:a:";
    int opt = ::getopt_long(argc, argv, optstr, options, nullptr);
    while (opt != -1)
    {
        switch (opt)
        {
            case 'q': proto = optarg; break;
            case 'p': port = std::atoi(optarg); break;
            case 'a': address = optarg; break;
            default: break;
        }

        opt = ::getopt_long(argc, argv, optstr, options, nullptr);
    }

    if (proto.empty() || port == 0)
    {
        std::cerr << "Invalid connection parameters" << std::endl;
        return 1;
    }

    try {
        if (proto == "tcp") initTcpClient(address, port);
        else if (proto == "udp") initUdpClient(address, port);
        else { std::cerr << "Invalid protocol" << std::endl; return 1; }
    }
    catch (const NetworkException& e)
    {
        std::cerr << "Network error occured: " << e.what() << " (" << e.errnum() << ")" << std::endl;
        return 1;
    }

    return 0;
}
