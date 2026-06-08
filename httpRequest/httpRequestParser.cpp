#include "httpRequestParser.hpp"

static std::string getFirstLine(const std::string& rawRequest, size_t &pos)
{
    pos = rawRequest.find("\r\n");

    if (pos == std::string::npos)
        return rawRequest;

    return rawRequest.substr(0, pos);
}



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
    firstLine >> method >> uri >> version;

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

void httpRequest::printHeaders() const
{
    std::map<std::string, std::string>::const_iterator it;

    for (it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << it->first << " => " << it->second << std::endl;
    }
}

int main()
{
    httpRequest req;

    std::string rawRequest =
    "POST /upload HTTP/1.1\r\n"
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

    std::cout << "\nHeaders:\n";
    req.printHeaders();
}