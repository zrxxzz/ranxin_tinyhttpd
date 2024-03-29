#include "connection_handler.hpp"
#include "response.hpp"

struct stat st; //记录静态资源文件状态
int cgi = 0; //判断是否需要执行CGI

bool isStaticResource(std::string& file){
    file="./htdocs"+file;
    
    if(stat(file.c_str(),&st)==-1){// 不存在
        std::cout<<"static source: "<<file<<" not exist!"<<std::endl;
        return false;
    }else 
    {   // 存在
        std::cout<<file<<" exist"<<std::endl;
   
        if ((st.st_mode & S_IFMT) == S_IFDIR){ // 是目录
            file+="/index.html";
            if(stat(file.c_str(),&st)==-1){
                std::cout<<"this is just a dir"<<std::endl;
                return false;
            }else return true;
        }else{ // 不是目录
            if ((st.st_mode & S_IXUSR) ||
            (st.st_mode & S_IXGRP) ||
            (st.st_mode & S_IXOTH)    ){
                cgi = 1;
                }
            return true;
        }
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
    Response response(this->socket);
    try{
        if(request.method.compare("GET")==0){
            if(isStaticResource(request.path)){
                std::cout<<"the file path is:"<<request.path<<" and the cgi is:"<<cgi<<std::endl;
                if(cgi){
                    response.handleCgiFile(request);
                    cgi = 0;// CGI重置
                }else{
                    // 文件已经存在的前提下
                    response.handleStaticFile(request.path);
                }
            }else{
                response.notFound();
            }
        }else if(request.method.compare("POST")==0){
            if(isStaticResource(request.path)){
                std::cout<<"the file path is:"<<request.path<<" and the cgi is:"<<cgi<<std::endl;
                if(cgi){
                    response.handleCgiFile(request);
                    cgi = 0;// CGI重置
                }else{
                    // 暂时先不写其他的POST请求
                    response.notFound();
                }
            }else{
                response.notFound();
            }
        }else{
            throw std::runtime_error("UNDEFINED HTTP METHOD");
        }
        if(!cgi)response.sendResponse(socket);
    }catch(const std::exception& e){
        std::cerr<<"!!!!HERE IS AN ERROR:"<<e.what()<<std::endl;
    }
    

    close(socket); // 关闭客户端连接
}
