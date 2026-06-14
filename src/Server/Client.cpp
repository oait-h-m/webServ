#include "Client.hpp"

Client::Client(int fd, Server *server): socket_fd(fd), server(server)
{
    request_complete = false;
}
    
Client::~Client()
{}

int Client::readRequest()
{
    char buffer[1024];

    int read = recv(socket_fd, buffer, sizeof(buffer), 0);
    if (read > 0)
    {
        request_buffer.append(buffer, read);
        return 1;
    }
    else if (read == 0)
        return -1;
    return 0;
}

void Client::setRequest(bool value)
{
    request_complete = value;
}

std::string Client::getRequestBuffer()
{
    return request_buffer;
}

bool Client::getRequestComplete()
{
    return request_complete;
}

HttpRequest Client::getRequest()
{
    return request;
}