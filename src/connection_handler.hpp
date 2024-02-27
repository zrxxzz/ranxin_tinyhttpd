#ifndef CONNECTION_HANDLER_CLASS_H
#define CONNECTION_HANDLER_CLASS_H
#include <iostream>
#include <string>
#include <unistd.h> 
class ConnectionHandler {
private:
    int socket;
public:
    ConnectionHandler(int socket) : socket(socket) {}
    void handleRequest();
};


#endif