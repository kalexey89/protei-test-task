
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

    std::string input(MaxMessageSize, '\0');
    while (std::cin.getline(&input[0], MaxMessageSize))
    {
        socket.write(input.c_str(), std::cin.gcount(), writed);

        std::string output(writed, '\0');
        socket.read(&output[0], writed, readed);
        std::cout << output << std::endl;

        std::fill(input.begin(), input.begin() + writed, '\0');
    }
}


void initUdpClient(const std::string& host, InternetAddress::port_t port)
{
    InternetAddress addr(host, port);
    UdpSocket socket;

    size_t readed = 0;
    size_t writed = 0;

    std::string input(MaxMessageSize, '\0');
    while (std::cin.getline(&input[0], MaxMessageSize))
    {
        socket.write(input.data(), std::cin.gcount(), writed, addr);

        std::string output(writed, '\0');

        InternetAddress tmp = {};
        socket.read(&output[0], writed, readed, tmp);
        std::cout << output << std::endl;

        std::fill(input.begin(), input.begin() + writed, '\0');
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
