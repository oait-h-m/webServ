#ifndef CORE_HPP
#define CORE_HPP

#include <vector>
#include <map>
#include "Server.hpp"
#include "Client.hpp"

class Core {
private:
    int epoll_fd;
    int n_servers;
    std::vector<Server*> servers;
    std::map<int, Client*> all_clients;
    
    void setup_epoll();
    void accept_client(Server *server, int listening_fd);
    void remove_client(int fd);
    
public:
    Core();
    ~Core();
    
    void add_server(Server *server);
    void run(); 
};

#endif