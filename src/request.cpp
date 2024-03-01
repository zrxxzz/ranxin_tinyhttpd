#include "request.hpp"

void Request::parseRequest(const std::string& requestData){
    std::string version;
    std::istringstream requestStream(requestData);
    // DEBUG: test the http request
    //std::cout<<"here is request Data:"<<requestData<<std::endl;
    requestStream >> this->method >> this->path >> version; 
    // DEBUG: for testing HTTP method
    // std::cout<<"the method from the request: "<<this->method<<std::endl;
    // std::cout<<"the path from the request: "<<this->path<<std::endl;
    

}