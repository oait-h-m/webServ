#include "Core.hpp"


Core::Core():epoll_fd(-1), n_servers(0)
{}

Core::~Core()
{}

void Core::creatServers()
{
    std::vector<int>::iterator p_it;
    std::vector<std::string>::iterator n_it;
    std::vector<std::string>::iterator r_it;
    for (int i = 0; i < n_servers; i++)
    {
        Server *server = new Server(*p_it, *n_it, *r_it);
        servers.push_back(server);
        ++p_it;
        ++n_it;
        ++r_it;
    }
}