#include "request.hpp"

std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void Request::parseRequest(const std::string& requestData){
    std::string version;
    std::istringstream requestStream(requestData);
    // DEBUG: test the http request
    std::cout<<"here is request Data:"<<requestData<<std::endl;
    requestStream >> this->method >> this->path >> version; 
    auto result=split(this->path,'?');
    this->path=result[0];
    this->queryString=result.size()>1 ? result[1] : "";

    std::string line;
    while(getline(requestStream,line) && !line.empty()){
        std::string tmp="Content-Length:";
        size_t pos = line.find(tmp);
        if(pos != std::string::npos){
            this->contentLength = line.substr(pos + tmp.length()+1);
            // std::cout<<"CONTENT-LENGTH"<<lengthStr<<std::endl;
        }
    }
    // DEBUG: for testing HTTP method
    // std::cout<<"the method from the request: "<<this->method<<std::endl;
    // std::cout<<"the path from the request: "<<this->path<<std::endl;

}