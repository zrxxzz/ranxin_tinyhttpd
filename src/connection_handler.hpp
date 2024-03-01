#ifndef CONNECTION_HANDLER_CLASS_H
#define CONNECTION_HANDLER_CLASS_H
#include <iostream>
#include <string>

#include <sys/select.h> 
#include <sys/stat.h>
#include <unistd.h> 
#include <stdexcept>

class ConnectionHandler {
private:
    int socket;
public:
    ConnectionHandler(int _socket);
    void handleRequest();
};

// 判断静态资源是否存在
bool isStaticResource(std::string& file);

#endif