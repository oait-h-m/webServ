#include "Server.hpp"


Server::Server(int port, const std::string &name, const std::string &root): port(port), server_name(name), root_dir(root)
{}
Server::~Server()
{}

void Server::init()
{
    listening_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_fd < 0)
        throw std::runtime_error("Error: socket failed");
    
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(listening_fd, (struct sockaddr *)&server, sizeof(server)))
        throw std::runtime_error("Error: bind failed");

    if (listen(listening_fd, 10000))
        throw std::runtime_error("Error: listen failed");
}

int Server::getListeningFd() 
{
    return listening_fd;
}
int Server::getPort()
{
    return port;
}
std::string Server::getServerName()
{
    return server_name;
}
std::string Server::getRootDir()
{
    return root_dir;
}