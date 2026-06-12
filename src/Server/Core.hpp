#ifndef CORE_HPP
#define CORE_HPP

#include <vector>
#include <map>
#include "Server.hpp"
#include "Client.hpp"
#include <sys/epoll.h>


class Core {
private:
    int epoll_fd;
    int n_servers;
    std::vector<int> s_ports;
    std::vector<std::string> s_names;
    std::vector<std::string> s_roots;
    std::vector<Server*> servers;
    std::map<int, Client*> all_clients;
    
    void setupEpoll();
    void creatServers();
    void acceptClient(Server *server, int listening_fd);
    void removeClient(int fd);
    
public:
    Core();
    ~Core();
    
    void addServer(Server *server);
    void run(); 
};

#endif