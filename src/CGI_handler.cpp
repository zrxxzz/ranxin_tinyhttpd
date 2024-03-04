#include "CGI_handler.hpp"


CgiHandler::CgiHandler(const std::string& _path, const std::string& _method, const std::string& _queryString, const int& _contentLength, int _clientSocket) 
    : path(_path), method(_method), queryString(_queryString), contentLength(_contentLength), clientSocket(_clientSocket) {
}

void CgiHandler::setEnv() {
    setenv("REQUEST_METHOD", method.c_str(), 1);
    if (method == "GET") {
        setenv("QUERY_STRING", queryString.c_str(), 1);
    } else if (method == "POST") {
        setenv("CONTENT_LENGTH", std::to_string(contentLength).c_str(), 1);
    }
}

void CgiHandler::executeCgi() {
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
    } else if (pid == 0) { // Child process
        this->childProcess();
    } else { // Parent process
        this->parentProcess();
        waitpid(pid, &status, 0);
    }
}

void CgiHandler::childProcess() {
    dup2(cgi_output[1], STDOUT_FILENO);
    dup2(cgi_input[0], STDIN_FILENO);
    close(cgi_output[0]);
    close(cgi_input[1]);

    setEnv();

    execl(path.c_str(), path.c_str(), (char*)NULL);
    perror("execl failed");
    exit(0);
}

void CgiHandler::parentProcess() {
    char c;
    std::string header = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\n";
    send(clientSocket, header.c_str(), header.length(), 0);
    close(cgi_output[1]);
    close(cgi_input[0]);
    if (method == "POST") {
        int content_length = contentLength;
        if (content_length > 0) {
            std::vector<char> buffer(content_length);
            if (recv(clientSocket, buffer.data(), buffer.size(), 0) > 0) {
                write(cgi_input[1], buffer.data(), buffer.size());
            }
        }
    }

    while (read(cgi_output[0], &c, 1) > 0) {
        send(clientSocket, &c, 1, 0);
    }
    close(cgi_output[0]);
    close(cgi_input[1]);
}
