#ifndef REQUEST_CLASS_H
#define REQUEST_CLASS_H
#include <string>
#include <sstream> 
class Request {
public:
    std::string method;
    std::string path;
    std::string queryString;

    // 解析从客户端接收到的原始请求数据
    static Request parseRequest(const std::string& requestData);
};

#endif
