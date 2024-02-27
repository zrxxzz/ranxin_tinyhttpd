#include "connection_handler.hpp"
#include "request.hpp"
#include "response.hpp"
void ConnectionHandler::handleRequest() {
    const int bufferSize = 1024;
    char buffer[bufferSize];
    std::string requestData;
    // 读取请求数据
    ssize_t bytesReceived = read(socket, buffer, bufferSize - 1);
    if (bytesReceived < 0) {
        std::cerr << "Error reading request from client." << std::endl;
        return;
    }
    std::cout<<bytesReceived<<std::endl;

    buffer[bytesReceived] = '\0'; // 确保字符串结束
    requestData = buffer;

    
    // 解析请求
    Request req = Request::parseRequest(requestData);

    // 创建响应
    Response res;
    res.setHeader("Content-Type", "text/html");
    res.setBody("<html><body><h1>Hello, World!</h1></body></html>");

    // 发送响应
    std::string responseStr = res.toString();
    write(socket, responseStr.c_str(), responseStr.length());

    close(socket); // 关闭客户端连接
}