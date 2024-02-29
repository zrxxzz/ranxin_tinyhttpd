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
    ConnectionHandler(int _socket){
        std::cout<<"the value:"<<this->socket<<std::endl;
        *(&this->socket)=_socket;
        std::cout<<"the value:"<<this->socket<<std::endl;
        };
    void handleRequest();
};


#endif