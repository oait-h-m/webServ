#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <vector>

class Server {
private:
    int listening_fd;
    int port;
    std::string server_name;
    std::string root_dir;
    
public:
    Server(int port, const std::string &name, const std::string &root);
    ~Server();
    
    void init();
    int getListeningFd() ;
    int getPort();
    std::string getServerName() ;
    std::string getRootDir() ;
};

#endif