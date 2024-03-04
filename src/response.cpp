#include "response.hpp"
#include <iostream> // 如果需要的话，可以用于调试

// 构造函数
Response::Response(int _socket) : clientSocket(_socket) {
    header = "Server: ranxin_httpd/0.1\r\nContent-Type: text/html\r\n\n";
}

// 设置响应体
void Response::setBody(const std::string& responseBody) {
    body = responseBody;
}

// 将响应转换为字符串
std::string Response::toString() const {
    return "HTTP/1.0 200 OK\r\n" + header + body;
}

// 处理静态文件
void Response::handleStaticFile(const std::string& path) {
    std::ifstream file(path);
    if(file) {   
        std::ostringstream ss;
        ss << file.rdbuf();
        std::string fileContent = ss.str();
        resBuf = "";
        this->setBody(fileContent);
        resBuf += this->toString();
    } else {
        throw std::runtime_error("FILE NOT FOUND!!!");
    }
}

// 处理CGI文件
void Response::handleCgiFile(const Request& request) {
    CgiHandler cgiHandler(request.path, request.method, request.queryString, request.contentLength, clientSocket);
    cgiHandler.executeCgi();
}

// 404未找到响应
void Response::notFound() {
    resBuf = "";
    resBuf += "HTTP/1.0 404 NOT FOUND\r\n"
              "Server: ranxin_httpd/0.1\r\n"
              "Content-Type: text/html\r\n\n"
              "<HTML><TITLE>Not Found</TITLE>\r\n"
              "<BODY><P>The server could not fulfill\r\n"
              "your request because the resource specified\r\n"
              "is unavailable or nonexistent.\r\n"
              "</BODY></HTML>\r\n";
}

// 发送响应
void Response::sendResponse(int client_fd) {
    send(client_fd, resBuf.c_str(), resBuf.length(), 0);
}
