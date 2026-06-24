#include "Client.hpp"

Client::Client(int fd, Server *server): socket_fd(fd), server(server)
{
    request_complete = false;
    response_sent = false;
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
    else if (read <= 0)
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

HttpRequest& Client::getRequest()
{
    return request;
}

int Client::getSocketFd()
{
    return socket_fd;
}

Server* Client::getServer()
{
    return server;
}

void Client::handleRequest()
{
    try
    {
        request.validate();

        if (request.getStatusCode() != 200)
        {
            response.setStatus(request.getStatusCode());
            response.setBody("Error");
        }
        else if (request.getMethod() == "GET")
            response = get_handler.execute(request);

        else if (request.getMethod() == "POST")
            response = post_handler.execute(request);

        else if (request.getMethod() == "DELETE")
            response = delete_handler.execute(request);

        else
        {
            response.setStatus(405);
            response.setBody("Method Not Allowed");
        }

        response.setHeader("Connection", "close");
    }
    catch (...)
    {
        response.setStatus(500);
        response.setBody("Internal Server Error");
    }
}

void Client::sendResponse()
{
    std::string res = response.build();
    size_t total_sent = 0;

    while (total_sent < res.size())
    {
        ssize_t sent = send(socket_fd, res.c_str() + total_sent, res.size() - total_sent, 0);
        if (sent <= 0)
            return;
        total_sent += sent;
    }
    response_sent = true;
}

bool Client::getResponseSent()
{
    return response_sent;
}

void Client::setResponseSent(bool value)
{
    response_sent = value;
}