#ifndef HTTPREQUESTPARSER
#define HTTPREQUESTPARSER

#include <iostream>
#include <sstream>
#include <string>
#include <map>

class httpRequest
{
private:
    std::string method;
    std::string uri;
    std::string version;

    std::map<std::string, std::string> headers;
    std::string body;

public:
    void parse(const std::string& rawRequest);

    const std::string& getMethod() const;
    const std::string& getUri() const;
    const std::string& getVersion() const;
    void printHeaders() const;
};

#endif