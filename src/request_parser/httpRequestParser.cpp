#include "httpRequestParser.hpp"

httpRequest::httpRequest() : statusCode(0) {}

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

const int&   httpRequest::getStatusCode() const
{
    return statusCode;
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
    {
        body = rawRequest.substr(pos + 4);

        if (headers.count("Content-Length"))
        {
            try
            {
                size_t len = std::stoul(headers["Content-Length"]);
                body = body.substr(0, len);
            }
            catch (...)
            {
                statusCode = 400;
                return;
            }
        }
    }
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

bool httpRequest::hasHostHeader() const
{
    return headers.count("Host") > 0;
}

bool httpRequest::isValidUri() const
{
    if (uri.empty() || uri[0] != '/')
        return false;
    return true;
}

void httpRequest::validate()
{
    if (statusCode != 0)
        return;
    if (!isValidMethod())
    {
        statusCode = 405;
        return;
    }

    if (!isValidVersion())
    {
        statusCode = 505;
        return;
    }

    if (!hasHostHeader())
    {
        statusCode = 400;
        return;
    }

    if (!isValidUri())
    {
        statusCode = 400;
        return;
    }

    statusCode = 200;
}


int main()
{
    httpRequest req;

    std::string rawRequest =
    "POST /upload HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "Content-Length: abc\r\n"
    "\r\n"
    "username=john&password=1234";


    req.parse(rawRequest);
    req.validate();
    std::cout << "Method: " << req.getMethod() << std::endl;
    std::cout << "URI: " << req.getUri() << std::endl;
    std::cout << "Version: " << req.getVersion() << std::endl;
    std::cout << "Body: " << req.getBody() << std::endl;
    std::cout << "status Code: " << req.getStatusCode() << std::endl;

}