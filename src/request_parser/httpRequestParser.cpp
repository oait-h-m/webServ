#include "httpRequestParser.hpp"

HttpRequest::HttpRequest() : statusCode(0) {}

const std::string& HttpRequest::getMethod() const
{
    return method;
}
const std::string& HttpRequest::getUri() const
{
    return uri;
}

const std::string& HttpRequest::getVersion() const
{
    return version;
}


const std::string& HttpRequest::getBody() const
{
    return body;
}

const int&   HttpRequest::getStatusCode() const
{
    return statusCode;
}

void HttpRequest::parse(const std::string& rawRequest)
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

    while (std::getline(stream, line) make                                                                                      2 ↵
make: *** No targets specified and no makefile found.  Stop.)
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
                size_t len = std::atoi(headers["Content-Length"].c_str());
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

bool HttpRequest::isValidMethod() const
{
    if (method == "GET" || method == "POST" || method == "DELETE")
        return true;
    return false;
}

bool HttpRequest::isValidVersion() const
{
    if (version == "HTTP/1.0" || version == "HTTP/1.1")
        return true;
    return false;
}

bool HttpRequest::hasHostHeader() const
{
    return headers.count("Host") > 0;
}

bool HttpRequest::isValidUri() const
{
    if (uri.empty() || uri[0] != '/')
        return false;
    return true;
}

void HttpRequest::validate()
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
