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
}

int main()
{
    httpRequest req;

    std::string rawRequest =
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: Mozilla/5.0\r\n"
        "Content-Type: text/html\r\n"
        "\r\n";

    req.parse(rawRequest);

    std::cout << "Method: " << req.getMethod() << std::endl;
    std::cout << "URI: " << req.getUri() << std::endl;
    std::cout << "Version: " << req.getVersion() << std::endl;

}