#include "Client.hpp"

Client::Client(int fd, Server *server): socket_fd(fd), server(server)
{
    request_complete = false;
}
    
Client::~Client()
{}

void Client::readRequest()
{
    char buffer[1024];

    int read = recv(socket_fd, buffer, sizeof(buffer), 0);
    if (read < 0)
    {
        
    }

}

std::string Client::getRawRequest()
{
    return request_buffer;
}