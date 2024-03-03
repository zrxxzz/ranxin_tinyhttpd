#ifndef RESPONSE_CLASS_H
#define RESPONSE_CLASS_H
#include <string>
#include <sys/socket.h>
#include <fstream>
#include <stdexcept>
#include "request.hpp"
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
        // 设置环境变量
        // std::string meth_env="REQUEST_METHOD="+method;
        setenv("REQUEST_METHOD",request.method.c_str(),1); 
        // 第三个参数1表示，如果环境变量已存在，则替代
        setenv("QUERY_STRING",request.queryString.c_str(),1);
        setenv("CONTENT_LENGTH",request.conternLength.c_str(),1);
        // 开管道和子进程
        int pipefd[2];
        pid_t pid;
        if (pipe(pipefd) == -1) {
            perror("pipe");
            throw std::runtime_error("PIPE BUILD FAILED!");
        }

        pid = fork();
        if (pid == -1) {
            perror("fork");
            throw std::runtime_error("FORK FAILED!");
        }
        if(pid==0){// 子进程

        }else{// 父进程

        }
        std::cout<<"here the pid:"<<pid<<std::endl;
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