#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <sys/socket.h>
#include "../request_parser/httpRequestParser.hpp"

class Server; 

class Client {
private:
    int socket_fd;
    Server *server; 
    std::string request_buffer;
    HttpRequest request;
    bool request_complete;
    
public:
    Client(int fd, Server *server); 
    ~Client();
    
    void readRequest();
    void handleRequest();
    void sendResponse();
    std::string getRawRequest();
    void    setRequest(bool request_complete);
    
    int getSocketFd();
    Server* getServer();
};

#endif