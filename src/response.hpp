#ifndef RESPONSE_CLASS_H
#define RESPONSE_CLASS_H
#include <string>

class Response {
public:
    std::string header;
    std::string body;

    void setHeader(const std::string& key, const std::string& value) {
        header += key + ": " + value + "\r\n";
    }

    void setBody(const std::string& responseBody) {
        body = responseBody;
    }

    std::string toString() const {
        return "HTTP/1.1 200 OK\r\n" + header + "\r\n" + body;
    }
};

#endif