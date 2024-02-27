#include "server.hpp"

Server::Server(int port){
        // 初始化地址结构
        address.sin_family = AF_INET; // IPv4
        address.sin_addr.s_addr = INADDR_ANY; // 任意地址
        address.sin_port = htons(port); // 端口
        addrlen = sizeof(address);
    }

bool Server::start() {
        // 创建套接字
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("socket failed");
            return false;
        }
        
        // 设置套接字选项
        int yes = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
            perror("setsockopt");
        }

        // 绑定套接字到服务器地址
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            perror("bind failed");
            return false;
        }

        // 开始监听
        if (listen(server_fd, 3) < 0) { // 同时可以有3个等待连接的请求
            perror("listen failed");
            return false;
        }

        std::cout << "Server started listening " << std::endl;
        return true;
    }

int Server::acceptConnection() {
    std::cout<<"hello"<<std::endl;
    int new_socket=accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if(new_socket<0){
        throw std::runtime_error("Accept failed");
    }
    return new_socket;
}

Server::~Server() {
        if (server_fd) {
            close(server_fd);
            std::cout << "Server shut down" << std::endl;
        }
    }