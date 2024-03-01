#include "connection_handler.hpp"
#include "request.hpp"
#include "response.hpp"
#include "static_file_handler.hpp"

struct stat st; //记录静态资源文件状态
int cgi = 0; //判断是否需要执行CGI

bool isStaticResource(std::string& file){
    file="."+file;
    std::cout<<"the file path is:"<<file<<std::endl;
    if(stat(file.c_str(),&st)==-1){
        std::cout<<"static source not exist!"<<std::endl;
        return false;
    }else 
    {
        std::cout<<"path exist"<<std::endl;
        if ((st.st_mode & S_IFMT) == S_IFDIR){
            file+="/index.html";
            if(stat(file.c_str(),&st)==-1){
                std::cout<<"this is just a dir"<<std::endl;
                return false;
            }
            else return true;
        }
        if ((st.st_mode & S_IXUSR) ||
                (st.st_mode & S_IXGRP) ||
                (st.st_mode & S_IXOTH)    )
            cgi = 1;
        return true;
    }
}

ConnectionHandler::ConnectionHandler(int _socket) : socket(_socket) {
        std::cout << "the socket_fd : " << this->socket << std::endl;
    }   
    
void ConnectionHandler::handleRequest() {
    const int bufferSize = 1024;
    char buffer[bufferSize];
    std::string requestData;
    // DEBUG：for socket_fd
    //std::cout<<"the socket from client:"<<this->socket<<std::endl;
    
    ssize_t bytesRead = read(socket, buffer, bufferSize - 1);
    if (bytesRead < 0) {
        std::cerr << "Failed to read data from client." << std::endl;
        return;
    }

    // 确保字符串正确终止
    buffer[bytesRead] = '\0';
    requestData = std::string(buffer);

    Request request;
    request.parseRequest(requestData);
    // DEBUG: for testing HTTP method
    // std::cout<<"the method from the request: "<<request.method<<std::endl;
    // std::cout<<"the path from the request: "<<request.path<<std::endl;
    Response response;
    StaticFileHandler staticFileHandler;
    try{
        if(request.method.compare("GET")==0){
            if(isStaticResource(request.path)){
                if(cgi){
                    response.handleCgiFile(request.path);
                }else{
                    // DEBUG: tesing the file_path
                    // std::cout<<"i'm here and the path is:"<<request.path<<std::endl;
                    response.handleStaticFile(request.path);
                }
            }else{
                response.notFound();
                response.sendResponse(socket);
            }
        }else if(request.method.compare("POST")==0){
            if(isStaticResource(request.path)){
                if(cgi){

                }else{

                }
            }else{
                response.notFound();
                response.sendResponse(socket);
            }
        }else{
            throw std::runtime_error("UNDEFINED HTTP METHOD");
        }
    }catch(const std::exception& e){
        std::cerr<<"!!!!HERE IS AN ERROR:"<<e.what()<<std::endl;
    }
    

    close(socket); // 关闭客户端连接
}
