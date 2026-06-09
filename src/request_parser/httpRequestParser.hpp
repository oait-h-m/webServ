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
    int statusCode;


public:
    void parse(const std::string& rawRequest);

    const std::string& getMethod() const;
    const std::string& getUri() const;
    const std::string& getVersion() const;
    const std::string& getBody() const;

    bool isValidMethod() const;
    bool isValidVersion() const;
    bool hasHostHeader() const;
    int validate();


    void printHeaders() const;

};

#endif