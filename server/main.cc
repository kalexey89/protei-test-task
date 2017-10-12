
#include "internetaddress.h"
#include "networkexception.h"
#include "tcpserver.h"
#include "udpserver.h"
#include "logger.h"

#include <getopt.h>

#include <cstring>
#include <algorithm>
#include <regex>
#include <iostream>
#include <string>
#include <array>
#include <thread>
#include <csignal>

using namespace protei;

static constexpr size_t MaxMessageSize = 65507; // Max UDP message size

namespace {

void onTcpConnection(Socket::handle_t handle);
void onUdpConnection(UdpSocket &server);

TcpServer tcpServer(onTcpConnection);
UdpServer udpServer(onUdpConnection);

void handleSignal(int signum)
{
    tcpServer.close();
    udpServer.close();

    std::cout << std::endl << "Server stoped. Bye-Bye! =)" << std::endl;

    ::exit(signum);
}


void handleInput(const std::string& data, const InternetAddress& from)
{
    std::vector<long long> digits = {};

    const std::regex rx("(\\+|-)?[[:digit:]]+");
    const std::sregex_token_iterator end;
    std::sregex_token_iterator iter(data.begin(), data.end(), rx, 0);
    while (iter != end)
    {
        try { digits.push_back(std::stoi(*iter++)); }
        catch (const std::exception& e) { ; }
    }

    if (digits.size() != 0)
    {
        logger << std::endl << "Received " << digits.size() << " digits from " << from << std::endl;

        std::sort(digits.begin(), digits.end(), std::greater<long long>());
        logger << "Digits in descending form: ";
        for (const auto& digit : digits)
        {
            logger << digit << " ";
        }

        logger << std::endl;

        logger << "Min: " << digits.back() << ";  Max: " << digits.front() << std::endl;
        logger << "Digits sum: " << std::accumulate(digits.begin(), digits.end(), 0) << std::endl;
    }
}


void onTcpConnection(Socket::handle_t handle)
{
    TcpSocket client(handle);
    client.setKeepAlive(true);

    size_t writed = 0;
    size_t readed = 0;

    std::string input(MaxMessageSize, '\0');
    while ((client.read(&input[0], input.size(), readed)) != 0)
    {
        client.write(input.data(), readed, writed);

        if (input.find_first_of("0123456789") != std::string::npos)
            handleInput(input.substr(0, readed), client.remoteAddress());

        std::fill(input.begin(), input.begin() + readed, '\0');
    }
}


void onUdpConnection(UdpSocket& server)
{
    size_t writed = 0;
    size_t readed = 0;

    InternetAddress from = {};

    std::string input(MaxMessageSize, '\0');
    while ((server.read(&input[0], input.size(), readed, from)) != 0)
    {
        server.write(input.data(), readed, writed, from);

        if (input.find_first_of("0123456789") != std::string::npos)
            handleInput(input.substr(0, readed), from);
    }
}

} // namespace

int main(int argc, char* argv[])
{
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);
    std::signal(SIGABRT, handleSignal);

    // Default servers addresses
    InternetAddress tcpServerAddress("0.0.0.0", 8080);
    InternetAddress udpServerAddress("0.0.0.0", 9090);

    const struct option options[] = {
        { "tcpport", optional_argument, NULL, 't' },
        { "tcpaddr", optional_argument, NULL, 'a' },
        { "udpport", optional_argument, NULL, 'u' },
        { "udpaddr", optional_argument, NULL, 'y' },
        { NULL, no_argument, NULL, 0 }
    };

    const char* optstr = "t:a:u:y:";
    int opt = ::getopt_long(argc, argv, optstr, options, nullptr);
    while (opt != -1)
    {
        switch (opt)
        {
            case 't': tcpServerAddress.setPort(std::atoi(optarg)); break;
            case 'a': tcpServerAddress.setHost(optarg); break;
            case 'u': udpServerAddress.setPort(std::atoi(optarg)); break;
            case 'y': udpServerAddress.setHost(optarg); break;
            default: break;
        }

        opt = ::getopt_long(argc, argv, optstr, options, nullptr);
    }

    try {
        std::thread tcpServerTread = std::thread([&] () {
            tcpServer.listen(tcpServerAddress);
        });

        std::thread udpServerThread = std::thread([&] () {
            udpServer.listen(udpServerAddress);
        });

        logger << "TCP server listening on " << tcpServerAddress << std::endl;
        logger << "UDP server listening on " << udpServerAddress << std::endl;

        tcpServerTread.join();
        udpServerThread.join();
    }
    catch (const NetworkException& e)
    {
        std::cerr << "Network error occured: " << e.what() << " (" << e.errnum() << ")" << std::endl;
        return -1;
    }

    return 0;
}
