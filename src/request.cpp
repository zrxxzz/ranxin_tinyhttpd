#include "request.hpp"

Request Request::parseRequest(const std::string& requestData){
    Request req;
    std::istringstream requestStream(requestData);
    requestStream >> req.method >> req.path; // 简化处理，只解析方法和路径
    // 进一步解析可以提取查询字符串、HTTP版本等信息
    return req;
}