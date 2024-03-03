#ifndef CGI_HANDLER_CLASS_H
#define CGI_HANDLER_CLASS_H
#include <sys/socket.h>
#include <sys/wait.h>
#include <string>
#include <iostream>
class CgiHandler{
private:
    std::string method;
    std::string path;
    std::string queryString;
    std::string contentLength;
    int clientSocket;
public:
    CgiHandler(const std::string& _path, const std::string& _method, const std::string& _queryString, const std::string& _contentLength, int _clientSocket)
: path(_path), method(_method), queryString(_queryString), contentLength(_contentLength), clientSocket(_clientSocket) {
    setEnv();
}

    void setEnv(){
        setenv("REQUEST_METHOD",this->method.c_str(),1); 
        setenv("QUERY_STRING",this->queryString.c_str(),1);
        setenv("CONTENT_LENGTH",this->contentLength.c_str(),1);
        // 第三个参数1表示，如果环境变量已存在，则替代
    };

    void executeCgi(){
        int cgi_output[2];
        int cgi_input[2];
        pid_t pid;
        int status;
        if (pipe(cgi_output) == -1 || pipe(cgi_input) == -1) {
            perror("pipe");
            throw std::runtime_error("PIPE BUILD FAILED!");
        }

        pid = fork();
        if (pid == -1) {
            perror("fork");
            throw std::runtime_error("FORK FAILED!");
        }

        if(pid==0){// 子进程
            std::cout<<"I'm CHILD!!!!"<<pid<<std::endl;
        }else{// 父进程
            std::cout<<"I'm FATHER!!!!"<<pid<<std::endl;
        }
        
        std::cout<<"ALL SUCCESS!!!!"<<std::endl;

    }

};

#endif