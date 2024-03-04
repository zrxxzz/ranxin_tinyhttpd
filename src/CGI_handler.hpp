#ifndef CGI_HANDLER_CLASS_H
#define CGI_HANDLER_CLASS_H

#include <string>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

class CgiHandler {
private:
    std::string method;
    std::string path;
    std::string queryString;
    int contentLength;
    int clientSocket;
    int cgi_output[2];
    int cgi_input[2];

public:
    CgiHandler(const std::string& _path, const std::string& _method, const std::string& _queryString, const int& _contentLength, int _clientSocket);

    void executeCgi();

protected:
    void setEnv();
    void childProcess();
    void parentProcess();
};

#endif // CGI_HANDLER_CLASS_H
