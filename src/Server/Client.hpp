#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <sys/socket.h>
#include "../request_parser/httpRequestParser.hpp"
#include "../request_handler/HttpResponse.hpp"
#include "../methods/Delete.hpp"
#include "../methods/Get.hpp"
#include "../methods/Post.hpp"

class Server; 

class Client {
private:
    int socket_fd;
    Server *server; 
    std::string request_buffer;
    Delete delete_handler;
    Get get_handler;
    Post post_handler;
    HttpRequest request;
    HttpResponse response;
    bool request_complete;
    
public:
    Client(int fd, Server *server); 
    ~Client();
    
    int readRequest();
    void handleRequest();
    void sendResponse();
    std::string getRequestBuffer();
    void    setRequest(bool request_complete);
    bool getRequestComplete();
    HttpRequest& getRequest();
    int getSocketFd();
    Server* getServer();
};

#endif