#include "Core.hpp"


Core::Core():epoll_fd(-1), n_servers(0)
{}

Core::~Core()
{}

void Core::creatServers()
{
    std::vector<int>::iterator p_it;
    p_it = s_ports.begin();
    std::vector<std::string>::iterator n_it;
    n_it = s_names.begin();
    std::vector<std::string>::iterator r_it;
    r_it = s_roots.begin();
    for (int i = 0; i < n_servers; i++)
    {
        Server *server = new Server(*p_it, *n_it, *r_it);
        servers.push_back(server);
        ++p_it;
        ++n_it;
        ++r_it;
    }
}

void Core::setupEpoll()
{
    epoll_fd = epoll_create(1000);
    if (epoll_fd < 0)
        throw std::runtime_error("Error: epoll_create failed");
    
    std::vector<Server *>::iterator it;
    it = servers.begin();
    for (int i = 0; i < n_servers; i++)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = (*it)->getListeningFd();
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, (*it)->getListeningFd(), &ev))
            throw std::runtime_error("ERROR: epoll_ctl failed");
        ++it;
    }
}