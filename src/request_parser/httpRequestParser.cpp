#include "httpRequestParser.hpp"

const std::string& httpRequest::getMethod() const
{
    return method;
}
const std::string& httpRequest::getUri() const
{
    return uri;
}

const std::string& httpRequest::getVersion() const
{
    return version;
}


const std::string& httpRequest::getBody() const
{
    return body;
}

void httpRequest::parse(const std::string& rawRequest)
{
    std::istringstream stream(rawRequest);
    std::string line;

    if (!std::getline(stream, line))
        return;

    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);

    std::istringstream firstLine(line);

    if (!(firstLine >> method >> uri >> version))
    {
        statusCode = 400;
        return;
    }

    while (std::getline(stream, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if (line.empty())
            break;

        size_t pos = line.find(':');
        if (pos == std::string::npos)
            continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
            value.erase(0, 1);

        headers[key] = value;
    }
    size_t pos = rawRequest.find("\r\n\r\n");

    if (pos != std::string::npos)
        body = rawRequest.substr(pos + 4);
}

bool httpRequest::isValidMethod() const
{
    if (method == "GET" || method == "POST" || method == "DELETE")
        return true;
    return false;
}

bool httpRequest::isValidVersion() const
{
    if (version == "HTTP/1.0" || version == "HTTP/1.1")
        return true;
    return false;
}

int httpRequest::validate()
{
    if (!isValidMethod())
        return 405;

    if (!isValidVersion())
        return 505;

    if (!hasHostHeader())
        return 400;

    return 200;
}

int main()
{
    httpRequest req;

    std::string rawRequest =
    "POST  HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "Content-Length: 27\r\n"
    "\r\n"
    "username=john&password=1234";

    req.parse(rawRequest);
    std::cout << "Method: " << req.getMethod() << std::endl;
    std::cout << "URI: " << req.getUri() << std::endl;
    std::cout << "Version: " << req.getVersion() << std::endl;
    std::cout << "Body: " << req.getBody() << std::endl;


}