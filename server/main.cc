
#include "tcpsocket.h"
#include "tcpserversocket.h"
#include "internetaddress.h"
#include "networkexception.h"
#include "udpsocket.h"
#include "tcpserver.h"
#include "logger.h"

#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <sstream>
#include <array>
#include <future>
#include <functional>
#include <thread>
#include <sys/resource.h>
#include <unistd.h>

using namespace protei;

static constexpr size_t MaxMessageSize = ((1024 * 64) + 1); // 64kb + 1 byte for null terminator

void tcpHandler(Socket::Handle clientHandle)
{
    TcpSocket clientSocket(clientHandle, true);
    clientSocket.setBlocking(false);
    clientSocket.setKeepAlive(true);

    logger("> New connection from %s\n\n", clientSocket.remoteAddress().toString().c_str());

    std::array<char, MaxMessageSize> buffer = { '\0' };
    size_t readed = 0;
    size_t writed = 0;
    ssize_t result = 0;
    ssize_t sresult = 0;
    do
    {
        result = clientSocket.read(buffer.data(), buffer.size(), readed);
        logger("> Received %u bytes from %s:\n--------------------\n\n%s\n--------------------\n\n",
               readed, clientSocket.remoteAddress().toString().c_str(), buffer.data());


        do
        {
            sresult = clientSocket.write(buffer.data(), buffer.size(), writed);
        }
        while (writed < readed);

        std::fill(buffer.begin(), buffer.end(), '\0');
    }
    while (result > 0);

    logger("> Client %s disconnected!\n", clientSocket.remoteAddress().toString().c_str());

    clientSocket.disconnect();
}

int main(int argc, char* argv[])
{

    try {
        std::thread tcpServerThread = std::thread([] ()
        {
            TcpServer tcpServer(tcpHandler);
            tcpServer.start(InternetAddress(SpecialIpAddress::Any, 8888));
        });

        tcpServerThread.join();


//        UdpSocket server(InternetAddress("127.0.0.1", 8888));
//        //server.setBlocking(false);
//        while (true)
//        {
//            std::array<char, 100> buffer = { '\0' };
//            size_t readed = 0;
//            InternetAddress addr = {};
//            while (server.read(buffer.data(), buffer.size(), readed, addr) > 0)
//            {

//                std::cout << " readed: " << readed << std::endl;
//                std::cout << " message: " << std::endl << buffer.data() << std::endl;

//                size_t writed = 0;
//                server.write(buffer.data(), readed, writed, addr);
//            }
//        }



//        TcpServerSocket listener(InternetAddress("127.0.0.1", 8888));
//        //listener.setBlocking(false);
//        while (true)
//        {
//            TcpSocket clientSocket2;
//            //clientSocket.setCloseOnDelete(false);
//            InternetAddress clientAddress;
//            const Socket::Handle handle = listener.accept(clientSocket2, clientAddress);
//            {

//                std::cout << " new connection from: " << clientAddress << std::endl;

//                std::thread([handle]() {
//                    TcpSocket clientSocket(handle, true);
//                    clientSocket.setBlocking(false);
//                    clientSocket.setKeepAlive(true);

//                    ssize_t rv = 0;
//                    do
//                    {
//                    std::array<char, 100> buffer = { '\0' };
//                    size_t readed = 0;
//                    rv = clientSocket.read(buffer.data(), buffer.size(), readed);

//                    std::cout << " rv: " << rv << std::endl;

//                        //clientSocket.disconnect();

//                    std::cout << " readed bytes: " << readed << std::endl;
//                    std::cout << " message: " << std::endl << buffer.data() << std::endl;
//                    }
//                    while (rv > 0);

//                    clientSocket.disconnect();

//                    std::cout << " conecton closed! " << std::endl;
//                }).detach();
//                //client
//            }
//        }
    }
    catch (const protei::NetworkException& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
