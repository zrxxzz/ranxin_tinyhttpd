#ifndef CONNECTION_HANDLER_CLASS_H
#define CONNECTION_HANDLER_CLASS_H
#include <iostream>
#include <string>
#include <sys/select.h> 
#include <unistd.h> 
class ConnectionHandler {
private:
    int socket;
public:
    ConnectionHandler(int _socket);
    void handleRequest();
};


#endif