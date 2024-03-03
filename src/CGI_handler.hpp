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
    int cgi_output[2];
    int cgi_input[2];
public:
    CgiHandler(const std::string& _path, const std::string& _method, const std::string& _queryString, const std::string& _contentLength, int _clientSocket)
        : path(_path), method(_method), queryString(_queryString), contentLength(_contentLength), clientSocket(_clientSocket) {
    }

    void setEnv(){
        setenv("REQUEST_METHOD",this->method.c_str(),1); 
        if(this->method=="GET"){
            setenv("QUERY_STRING",this->queryString.c_str(),1);
        }else if(this->method=="POST"){
            setenv("CONTENT_LENGTH",this->contentLength.c_str(),1);
        }
            
        // 第三个参数1表示，如果环境变量已存在，则替代
    };

    void executeCgi(){
        
        pid_t pid;
        int status;
        if (pipe(cgi_output) == -1 || pipe(cgi_input) == -1) {
            perror("pipe");
            throw std::runtime_error("PIPE BUILD FAILED!");
        }
        execl(path.c_str(),path.c_str(),(char*)NULL);
        pid = fork();
        if (pid == -1) {
            perror("fork");
            throw std::runtime_error("FORK FAILED!");
            return ;
        }else if(pid==0){// 子进程
            std::cout<<"I'm CHILD!!!!"<<pid<<std::endl;
            this->childProcess();
               
        }else{// 父进程
            std::cout<<"I'm FATHER!!!!"<<pid<<std::endl;
            this->parentProcess();
            std::cout<<"PARENT DOWN!!!"<<std::endl;
            waitpid(pid,&status,0);
        }
        // std::cout<<"ALL SUCCESS!!!!"<<std::endl;
    }

protected:
    void childProcess(){
    // 管道重定向，关闭不需要的管道
        dup2(cgi_output[1],STDOUT_FILENO);
        // dup2(cgi_input[0],STDIN_FILENO);
        close(cgi_output[0]);
        // close(cgi_input[1]);
         
        setEnv();
         
        execl(path.c_str(),path.c_str(),(char*)NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
        
    };

    void parentProcess(){
        char c;
        std::string header="HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\n";
        char buffer[1024];
        // close(cgi_output[1]);
        // close(cgi_input[0]);
        // if(this->method=="POST"){
        //     for(int i=0;i<stoi(this->contentLength);i++){
        //         recv(this->clientSocket,&c,1,0);
        //         write(cgi_input[1],&c,1);
        //     }
        // }
        send(this->clientSocket,header.c_str(),header.length(),0);
        while (read(cgi_output[0], buffer, sizeof(buffer)) > 0) {
            send(this->clientSocket,buffer,sizeof(buffer),0);
        }
        // while(read(cgi_output[0],&c,1)>0){
        //     send(this->clientSocket,&c,1,0);
        // }
        close(cgi_output[0]);
        close(cgi_input[1]);
    }
};

#endif