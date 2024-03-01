#ifndef REQUEST_CLASS_H
#define REQUEST_CLASS_H
#include <iostream>
#include <string>
#include <sstream> 
class Request {
public:
    std::string method;
    std::string path;
    std::string queryString;

    // 解析请求
    void parseRequest(const std::string& requestData);
};

#endif
