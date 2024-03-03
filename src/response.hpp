#ifndef RESPONSE_CLASS_H
#define RESPONSE_CLASS_H
#include <string>
#include <sys/socket.h>
#include <fstream>
#include <stdexcept>
#include "request.hpp"
#include "CGI_handler.hpp"
class Response {
public:
    int clientSocket;
    std::string header = "Server : ranxin_httpd/0.1\r\nContent-Type: text/html\r\n\n";
    std::string body;
    std::string resBuf="";

    Response(int _socket) : clientSocket(_socket){};
    
    void setBody(const std::string& responseBody) {
        body = responseBody;
    }

    std::string toString() const {
        return "HTTP/1.0 200 OK\r\n" + header + body;
    }

    void handleStaticFile(const std::string& path){
        std::ifstream file(path);
        if(file){   
            std::ostringstream ss;
            ss<<file.rdbuf();
            std::string fileContent = ss.str();
            resBuf="";
            this->setBody(fileContent);
            resBuf+=this->toString();
            // DEBUG: testing response (static file)
            // std::cout<<"this response are as followed:"<<resBuf<<std::endl;
        }else{
            throw std::runtime_error("FILE NOT FOUND!!!");
        }
    };

    void handleCgiFile(const Request& request){
       CgiHandler cgiHandler(request.path,request.method,request.queryString,request.contentLength,clientSocket);
        cgiHandler.executeCgi();
    };

    void notFound(){
        resBuf="";
        resBuf+="HTTP/1.0 404 NOT FOUND\r\n"
        "Server : ranxin_httpd/0.1\r\n"
        "Content-Type: text/html\r\n\n"
        "<HTML><TITLE>Not Found</TITLE>\r\n"
        "<BODY><P>The server could not fulfill\r\n"
        "your request because the resource specified\r\n"
        "is unavailable or nonexistent.\r\n"
        "</BODY></HTML>\r\n";
    }

    void sendResponse(int client_fd){
        // DEBUG: test the response
        // std::cout<<"ERROR SOCKET_FD:"<<client_fd<<std::endl;
        // std::cout<<"the response buffer are:"<<resBuf<<std::endl;
        size_t i=send(client_fd,resBuf.c_str(),resBuf.length(),0);
    }
};

#endif