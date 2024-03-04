#ifndef RESPONSE_CLASS_H
#define RESPONSE_CLASS_H

#include <string>
#include <sys/socket.h>
#include <fstream>
#include <sstream> // 注意添加这个头文件，因为使用了ostringstream
#include "request.hpp" 
#include "CGI_handler.hpp" 

class Response {
public:
    int clientSocket;
    std::string header;
    std::string body;
    std::string resBuf;

    Response(int _socket);
    
    void setBody(const std::string& responseBody);

    std::string toString() const;

    void handleStaticFile(const std::string& path);

    void handleCgiFile(const Request& request);

    void notFound();

    void sendResponse(int client_fd);
};

#endif // RESPONSE_CLASS_H
