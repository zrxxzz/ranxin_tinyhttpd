#ifndef SERVER_CLASS_H
#define SERVER_CLASS_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>


class Server {
private:
    int server_fd; // 服务器套接字文件描述符
    struct sockaddr_in address; // 服务器地址
    int addrlen; // 地址长度

public:
    Server(int port);
    bool start();
    int acceptConnection();
    ~Server();
};

#endif


