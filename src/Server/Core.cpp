#include "Core.hpp"


Core::Core():epoll_fd(-1)
{}

Core::~Core()
{}

void Core::creatServers()
{
    std::vector<ServerConfig>::iterator it;
    for (it = server_config.server_configs.begin(); it != server_config.server_configs.end(); it++)
    {
        Server *server = new Server(it->port, it->server_name, it->root);
        servers.push_back(server);
    }
}

void Core::setupEpoll()
{
    epoll_fd = epoll_create(1000);
    if (epoll_fd < 0)
        throw std::runtime_error("Error: epoll_create failed");
    
    std::vector<Server *>::iterator it;
    for (it = servers.begin(); it != servers.end() ; it++)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = (*it)->getListeningFd();
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, (*it)->getListeningFd(), &ev))
            throw std::runtime_error("ERROR: epoll_ctl failed");
    }
}

int Core::isListeningFd(int fd)
{
    std::vector<Server *>::iterator it;
    for (it = servers.begin(); it != servers.end() ; it++)
    {
        if ((*it)->getListeningFd() == fd)
            return 1;
    }
    return 0;
}

Server* Core::getServer(int fd)
{
    std::vector<Server *>::iterator it;
    for (it = servers.begin(); it != servers.end() ; it++)
    {
        if ((*it)->getListeningFd() == fd)
            return *it;
    }
    return NULL;
}

void Core::acceptClient(Server* server)
{
    struct sockaddr_in client_info;
    socklen_t client_len = sizeof(client_info);

    int client_fd = accept(server->getListeningFd(), (struct sockaddr *)&client_info, &client_len);
    if (client_fd < 0)
    {
        std::cout << "ERROR: accept client faild for server:" << server->getListeningFd() << std::endl;
        return ;
    }
    struct epoll_event client_ev;
    client_ev.events = EPOLLIN;
    client_ev.data.fd = client_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev) < 0)
    {
        std::cerr << "epoll_ctl ADD client failed for server:" << server->getListeningFd() << std::endl;
        close(client_fd);
        return ;
    }
    Client *client = new Client(client_fd, server);
    all_clients[client_fd] = client;
}

void Core::removeClient(int fd)
{
    std::cout << "Client fd=" << fd << " disconnected" << std::endl;

    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
}

void Core::run()
{
    creatServers();

    std::vector<Server *>::iterator it;
    for (it = servers.begin(); it != servers.end(); it++)
        (*it)->init();
    
    setupEpoll();

    struct epoll_event events[10000];
    while (true)
    {
        int ready = epoll_wait(epoll_fd, events, 10000, -1);
        if (ready < 0){
            std::cout << "ERROR: epoll_wait failed\n";
            break;
        }
        for (int i = 0; i < ready; i++)
        {
            int fd = events[i].data.fd;
            uint32_t event_mask = events[i].events;

            if (isListeningFd(fd))
            {
                acceptClient(getServer(fd));
                continue;
            }
            else if (event_mask & EPOLLIN)
            {
                Client& client = *all_clients[fd];
                if (client.readRequest() <= 0)
                {
                    removeClient(fd);
                    continue;
                }
                client.getRequest().parse(client);
                if (!client.getRequestComplete())
                    continue;
                client.handleRequest();
            }


        }
    }
}