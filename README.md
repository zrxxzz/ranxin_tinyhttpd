# cpp项目 ---ranxin_tinyhttpd

## C语言版解析

> Tinyhttpd 是J. David Blackstone在1999年写的一个不到 500 行的超轻量型 Http Server

本项目主要是为了通过cpp复现一遍该项目

### 原项目主体逻辑

设置端口号--》服务器初始化--》 接收客户端连接--》运行CGI

### 服务器初始化

> mingw64中不提供<sys/socket.h>因为这是属于类unix系统的
>
> 可以在[第三方库文档](https://pubs.opengroup.org/onlinepubs/7908799/xns/socket.html)中进行查看

#### 创建套接字

通过<sys/socket.h>库中的socket函数创建socket

`httpd = socket(PF_INET, SOCK_STREAM, 0);`

其中的`PF_INET`和文档中的`AF_INET`几乎相同，不做赘述，详看[博客](https://blog.csdn.net/fivedoumi/article/details/72424784)

`SOCK_STREAM`是socket的类型，表示面向连接，文档数据传输（TCP）

![image-20240225145134319](https://typora-zrx.oss-cn-beijing.aliyuncs.com/img/2024/02/25/20240225-145137.png)

#### 初始化地址

通过结构体`sockaddr_in`实现地址的存储，具体内部结构如下：

```C
struct sockaddr_in {
    short            sin_family;   // 地址族(Address Family)，对于IPv4协议，总是设置为AF_INET。
    unsigned short   sin_port;     // 16位的端口号，使用网络字节顺序（大端序）。
    struct in_addr   sin_addr;     // 32位IP地址，存储网络中的主机地址。
    char             sin_zero[8];  // 未使用，为了与struct sockaddr保持大小一致而保留的空字节。
};
```

原项目如下：

```C
struct sockaddr_in name;
memset(&name, 0, sizeof(name));
name.sin_family = AF_INET;
name.sin_port = htons(*port);
name.sin_addr.s_addr = htonl(INADDR_ANY);
```

其中的`htons()`是为了将主机字节顺序转换为网络字节顺序（网络协议规定）

`htonl(INADDR_ANY);`是表示绑定到任意可用接口

#### 设置套接字选项

具体结构如下：

```C
int setsockopt(int socket, int level, int option_name, const void
                *option_value, socklen_t option_len);
```

`setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)`

`httpd` socket描述符

`level`：SOL_SOCKET 设置协议级别选项

`SO_REUSEADDR` 允许重用本地地址，

`&on`指向选项值的缓冲区的指针，用于启用（提供了一个指向实际选项值的指针

`sizeof(on)`指定了缓冲区大小

#### 绑定套接字和地址

`bind(httpd, (struct sockaddr *)&name, sizeof(name)`

将httpd socket描述符与name地址绑定

#### 动态分配端口

如果传入的port为0，则重新动态分配一个可用端口，通过getsockname函数来自动填充name地址（其中就包括了动态端口的分配

`getsockname(httpd, (struct *sockaddr* *)&name, &namelen)`

然后再把端口从网络字节顺序转换成主机字节顺序

`*port = ntohs(name.sin_port);`

#### 监听套接字

`listen(httpd, 5)`，5是监听队列的最大长度

开始监听后，返回socket描述符

### 处理客户端请求

主程序循环接收请求

```C++
client_sock = accept(server_sock,
                (struct sockaddr *)&client_name,
                &client_name_len);
        if (client_sock == -1)
            error_die("accept");
        /* accept_request(&client_sock); */
        if (pthread_create(&newthread , NULL, (void *)accept_request, (void *)(intptr_t)client_sock) != 0)
            perror("pthread_create");
```

具体结构如下

```C
int accept (int socket, struct sockaddr *address,
                                 socklen_t *address_len);
```

这里的socket就是服务端的套接字描述符，目的是为了和服务端的类型协议、地址族一致

后面的sockaddr和sockelen_t和之前的服务端类似，主要是设置客户端的地址

返回的依旧是socket文件描述符

#### 读取请求行

原函数`int get_line(int sock, char *buf, int size)`主要是通过socket.h提供的`recv`函数实现，结构如下

```C
ssize_t recv(int socket, void *buffer, size_t length, int flags);
```

需要注意的是读取过程中读到了'\r'回车符，需要预览下个字符，如果是换行符就把'\r'移除，如果不是，就把'\r'改为'\n'存入缓冲区

最后根据缓冲区size或者读到'\n'(或'\r\n')为结束

#### 解析请求行

方法比较简单，就是根据空格分隔符分割字符串，然后分别作为

- 请求方法
- URL

如果方法是POST则需要建立CGI，如果方法是GET则检查是否有查询参数

分别进行缓存

#### 构造资源路径

```c++
sprintf(path, "htdocs%s", url);
    if (path[strlen(path) - 1] == '/')
        strcat(path, "index.html");
```

先进行路径构造，如果无资源就返回NOT FOUND

如果有，通过位掩码来判断是否为目录，如果是目录就加默认页面(index.html)

如果不是则判断是否可执行，如果可执行则进行CGI调用

### CGI调用

#### 子进程

> 需要向父进程输出，所以要留子进程输出管道的写端

1. 重定向输入输出：
   - 通过STDOUT重定向，CGI可以通过管道cgi_output，将输出发送给父进程
   - 通过STDIN重定向，CGI可以通过管道cgi_input，从父进程读取数据

2. 关闭不需要的管道端口

3. 设置环境变量 REAUEST_METHOD、QUERY_STRING、CONTENT_LENGTH

4. 执行CGI脚本

#### 父进程

> 需要从子进程读CGI执行后的输出，所以要留子进程输出管道的读端
>
> 同时要写入子进程的输入管道

1. **重定向标准输入输出**：
   - `dup2(cgi_output[1], STDOUT);`：将标准输出（STDOUT）重定向到`cgi_output`管道的写端，这样CGI脚本的输出可以通过管道发送给父进程。
   - `dup2(cgi_input[0], STDIN);`：将标准输入（STDIN）重定向到`cgi_input`管道的读端，允许CGI脚本从父进程读取数据（例如POST请求的正文）。
2. **关闭不需要的管道端**：
   - 子进程关闭`cgi_output[0]`（管道的读端）和`cgi_input[1]`（管道的写端），因为子进程只需要写入输出并读取输入。
3. **设置环境变量**：
   - 设置`REQUEST_METHOD`环境变量为HTTP请求方法（GET或POST）。
   - 如果是GET请求，设置`QUERY_STRING`环境变量为URL中的查询字符串。
   - 如果是POST请求，设置`CONTENT_LENGTH`环境变量为请求体的长度。
4. **执行CGI脚本**：
   - 使用`execl`函数执行指定路径的CGI脚本。`execl`函数会替换当前进程的映像为指定的程序，这里就是CGI脚本。
   - 执行完毕后，子进程通过`exit(0)`正常退出。

---

### 补充

虽然socket描述在该项目中只是简单的整数，但这只是标识符，真正携带网络会话信息的是OS底层数据结构，描述符使程序能通过标准API与底层结构进行交互

> 详细介绍：
>
> 描述符实际上是操作系统提供的一个抽象，它代表了一个网络通信会话的端点。操作系统通过这个描述符来管理所有与该会话相关的信息，包括协议类型（如TCP或UDP）、本地和远程的IP地址和端口号、连接状态、缓冲区信息等。


---

## CPP重构版

> 正片开始！

### 重构策略

- **面向对象设计**：每个类有明确的职责，通过类和对象的组合来完成服务器的功能。
- **线程池**：为了提高效率，可以使用线程池来管理`ConnectionHandler`的实例，避免频繁创建和销毁线程的开销。
- **异常处理**：使用C++异常处理机制来处理错误，提高代码的健壮性。
- **智能指针**：使用智能指针管理动态分配的内存，减少内存泄漏的风险。
- **STL**：使用标准模板库（STL）提供的数据结构和算法，如`std::vector`, `std::map`等。

### 主体设计结构

> 下列代码只放hpp定义，具体实现请前往仓库（文章最底部）

#### 主体架构图

![image-20240304210631380](https://typora-zrx.oss-cn-beijing.aliyuncs.com/img/2024/03/04/20240304-210634.png)

#### main 函数

```c++
#include <iostream>
#include <memory>
#include <thread>
#include "Server.h"
#include "ConnectionHandler.h"

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
            }

            // 使用智能指针管理ConnectionHandler，确保资源正确释放
            std::shared_ptr<ConnectionHandler> handler(new ConnectionHandler(clientSocket));

            // 创建一个线程来处理连接，实现并发处理
            std::thread([handler]() {
                handler->handleRequest();
            }).detach(); // 将线程分离，让它独立执行
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception caught in main: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception caught in main" << std::endl;
    }

    return 0;
}

```

> 注意，这里的Lambda表达式捕获时使用的是值捕获，如果是引用捕获，中间shared_ptr如果执行或修改了参数，那么表达式内部也会发生变化（socket描述符发生变化，通道无法建立）

> 同时，不能直接使往thread传入函数指针，因为这里调用的是对象的成员函数，因此传入指针前必须先捕获对象，这就要求使用lambda表达式了

#### 1. **Server类**

- **职责**：负责服务器的初始化、启动、监听。

- 方法

  ：

  - `start()`: 配置服务器，绑定端口，监听连接。
  - `acceptConnection()`: 等待并接受客户端连接。

```C++
#ifndef SERVER_CLASS_H
#define SERVER_CLASS_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>


class Server {
private:
    int server_fd; // 服务器套接字文件描述符
    struct sockaddr_in address; // 服务器地址
    int addrlen; // 地址长度

public:
    Server(int port);
    bool start();
    int acceptConnection();
    ~Server();
};

#endif
```

#### 2. **ConnectionHandler类**

- **职责**：处理单个客户端连接。

- 方法

  ：

  - `handleRequest()`: 处理客户端的HTTP请求。
  - `isStaticResource();`: 判断静态资源是否存在

```c++
#ifndef CONNECTION_HANDLER_CLASS_H
#define CONNECTION_HANDLER_CLASS_H
#include <iostream>
#include <string>

#include <sys/select.h> 
#include <sys/stat.h>
#include <unistd.h> 
#include <stdexcept>

class ConnectionHandler {
private:
    int socket;
public:
    ConnectionHandler(int _socket);
    void handleRequest();
};

bool isStaticResource(std::string& file);

#endif
```

#### 3. **Request类**

- **职责**：解析和存储HTTP请求信息。

- 属性

  ：

  - HTTP方法（GET、POST等）
  - URL
  - 查询字符串
  - 报文头
  - 报文体

- 方法

  ：

  - `parseRequest()`: 从客户端连接中读取并解析HTTP请求。

```c++
#ifndef REQUEST_CLASS_H
#define REQUEST_CLASS_H
#include <iostream>
#include <string>
#include <sstream> 
#include <vector>
class Request {
public:
    std::string method;
    std::string path;
    std::string queryString;
    int contentLength;
    std::string content;
    // 解析请求
    void parseRequest(const std::string& requestData);
};

#endif
```

####  4. **Response类**

- **职责**：构建和存储HTTP响应信息，发送响应。

- 方法

  ：

  - `setBody()`: 设置响应体。
  - `handleStaticFile`: 处理静态文件
  - `handleCgiFile`:处理CGI文件
  - `sendResponse`:发送响应
  - `notFound()`:处理其他情况

```c++
#ifndef RESPONSE_CLASS_H
#define RESPONSE_CLASS_H

#include <string>
#include <sys/socket.h>
#include <fstream>
#include <sstream> // 注意添加这个头文件，因为使用了ostringstream
#include "request.hpp" 
#include "CGI_handler.hpp" 

class Response {
public:
    int clientSocket;
    std::string header;
    std::string body;
    std::string resBuf;

    Response(int _socket);
    
    void setBody(const std::string& responseBody);

    std::string toString() const;

    void handleStaticFile(const std::string& path);

    void handleCgiFile(const Request& request);

    void notFound();

    void sendResponse(int client_fd);
};

#endif // RESPONSE_CLASS_H
```

#### 5. **CGIHandler类**

- **职责**：执行CGI脚本并处理其输出。

- 方法

  ：

  - `executeCgi()`: 处理CGI脚本
  - `setEnv()`: 设置环境变量
  - `childProcess()`: 子进程处理
  - `parentProcess()`: 父进程处理

```c++
#ifndef CGI_HANDLER_CLASS_H
#define CGI_HANDLER_CLASS_H

#include <string>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

class CgiHandler {
private:
    std::string method;
    std::string path;
    std::string queryString;
    int contentLength;
    int clientSocket;
    int cgi_output[2];
    int cgi_input[2];

public:
    CgiHandler(const std::string& _path, const std::string& _method, const std::string& _queryString, const int& _contentLength, int _clientSocket);

    void executeCgi();

protected:
    void setEnv();
    void childProcess();
    void parentProcess();
};

#endif // CGI_HANDLER_CLASS_H
```

#### 7. **Logger类**

- **职责**：提供日志记录功能。

- 方法

  ：

  - `log()`: 记录日志信息。

> 暂无

### ACHIEVE

![image-20240304212530770](https://typora-zrx.oss-cn-beijing.aliyuncs.com/img/2024/03/04/20240304-213347.png)

![image-20240304212546887](https://typora-zrx.oss-cn-beijing.aliyuncs.com/img/2024/03/04/20240304-213350.png)

![image-20240304212609978](https://typora-zrx.oss-cn-beijing.aliyuncs.com/img/2024/03/04/20240304-213352.png)

### Q&A

- Q：为什么每次都是用 **const** type **&** 进行传参

  A：const 是为了防止代码被修改，& 是减少整个拷贝需要的开销（内存、时间），总体来说是为了安全和性能 

- Q：为什么要使用shared_ptr 来处理client连接

  A：

  - `std::shared_ptr`的引用计数机制是线程安全的，这意味着它可以安全地在多个线程间共享
  - `std::shared_ptr`提供自动的内存管理功能，它会跟踪引用计数，当没有任何`shared_ptr`对象指向当前资源时，它会自动释放该资源
  - 具体详见下一章CPP学习日记（五）

### TODO

- 线程池管理client
- 编写Logger类保存日志
- 利用事件循环机制（select、poll、epoll）实现I/O多路复用

### ISSUE

- POST带请求体无法传递给管道





### 参考资料

[Tinyhttpd仓库](https://github.com/zrxxzz/Tinyhttpd)(fork的)

[socket库文档](https://pubs.opengroup.org/onlinepubs/7908799/xns/socket.html)

[地址库文档](https://pubs.opengroup.org/onlinepubs/009695399/basedefs/netinet/in.h.html)

附上[本项目仓库](https://github.com/zrxxzz/ranxin_tinyhttpd)
