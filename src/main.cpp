#include <memory>
#include <thread>
#include "server.hpp"
#include "connection_handler.hpp"
#include "threadPool.hpp"
int main() {
    try {
        Server server(4000); // 创建Server实例，监听端口4000
        server.start(); // 启动服务器，开始监听端口

        std::cout << "Server started on port 4000" << std::endl;

        while (true) {
            // 等待并接受客户端连接
            int clientSocket = server.acceptConnection();
            if (clientSocket < 0) {
                std::cerr << "Failed to accept client connection" << std::endl;
                continue;
            }else{
                // DEBUG: for socket_fd
                //std::cout<<"the socket from client"<<clientSocket<<std::endl;
                std::shared_ptr<ConnectionHandler> handler(new ConnectionHandler(clientSocket));

                threadPool pool(4);

                auto res=pool.enqueue([&handler]{ // 值捕获和引用捕获看个人需求吧
                    handler->handleRequest();
                });
                
                // res.get(); //future 类可以返回调用结果
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception caught in main: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception caught in main" << std::endl;
    }

    return 0;
}
