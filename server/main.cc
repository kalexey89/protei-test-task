
#include "tcpsocket.h"
#include "tcpserversocket.h"
#include "internetaddress.h"
#include "networkexception.h"
#include "udpsocket.h"
#include "tcpserver.h"
#include "udpserver.h"
#include "logger.h"

#include <arpa/inet.h>

#include <cctype>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <regex>
#include <iostream>
#include <string>
#include <sstream>
#include <array>
#include <future>
#include <functional>
#include <thread>
#include <sys/resource.h>
#include <unistd.h>
#include <csignal>
#include <cstring>
#include <getopt.h>

using namespace protei;

static constexpr size_t MaxMessageSize = ((1024 * 64) + 1); // 64kb + 1 byte for null terminator

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
        client.write(input.data(), input.size(), writed);

        if (input.find_first_of("0123456789") != std::string::npos)
            handleInput(input.substr(0, readed), client.remoteAddress());

        std::fill(input.begin(), input.end(), '\0');
    }
}


void onUdpConnection(UdpSocket& server)
{
    size_t writed = 0;
    size_t readed = 0;

    InternetAddress client = {};

    std::string input(MaxMessageSize, '\0');
    while ((server.read(&input[0], input.size(), readed, client)) != 0)
    {
        server.write(input.data(), input.size(), writed, client);

        if (input.find_first_of("0123456789") != std::string::npos)
            handleInput(input.substr(0, readed), client);
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

    const char* optstr = "t:a:u:x:";
    int opt = ::getopt_long(argc, argv, optstr, options, nullptr);
    while (opt != -1)
    {
        switch (opt)
        {
            case 't': tcpServerAddress.setPort(std::atoi(optarg)); break;
            case 'a': tcpServerAddress.setHost(optarg); if (optarg == nullptr) std::printf("optarg\n"); break;
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
