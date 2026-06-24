#ifndef CORE_HPP
#define CORE_HPP

#include <vector>
#include <map>
#include "Server.hpp"
#include "../request_parser/httpRequestParser.hpp"
#include "Client.hpp"
#include <sys/epoll.h>
#include <unistd.h>
#include "../config_file_parser/ConfigParser.hpp"


class Core {
private:
    int epoll_fd;
    WebServerConfig server_config;
    std::vector<Server*> servers;
    std::map<int, Client*> all_clients;
    
    void setupEpoll();
    int  isListeningFd(int fd);
    Server* getServer(int fd);
    void creatServers();
    void  acceptClient(Server *server);
    void removeClient(int fd);
    
public:
    Core(std::string config_path);
    ~Core();
    
    void run(); 
};

#endif