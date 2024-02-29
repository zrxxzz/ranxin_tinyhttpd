#include "connection_handler.hpp"
#include "request.hpp"
#include "response.hpp"

ConnectionHandler::ConnectionHandler(int _socket) : socket(_socket) {
        std::cout << "the socket value: " << this->socket << std::endl;
    }   
    
void ConnectionHandler::handleRequest() {
    const int bufferSize = 1024;
    char buffer[bufferSize];
    std::string requestData;
    std::cout<<"the socket from client:"<<this->socket<<std::endl;
    // 尝试读取数据
    ssize_t bytesRead = read(socket, buffer, bufferSize - 1);
    if (bytesRead < 0) {
        std::cerr << "Failed to read data from client." << std::endl;
        return;
    }

    // 确保字符串正确终止
    buffer[bytesRead] = '\0';
    requestData = std::string(buffer);

    std::cout << "Received request: " << requestData << std::endl;

    close(socket); // 关闭客户端连接
}