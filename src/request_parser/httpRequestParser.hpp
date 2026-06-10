#ifndef HTTPREQUESTPARSER_HPP
#define HTTPREQUESTPARSER_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <map>

class HttpRequest
{
private:
    std::string method;
    std::string uri;
    std::string version;

    std::map<std::string, std::string> headers;
    std::string body;
    int statusCode;

    HttpRequest(const HttpRequest &other);
    HttpRequest &operator=(const HttpRequest &other);

public:
    HttpRequest();
    void parse(const std::string& rawRequest);

    const std::string& getMethod() const;
    const std::string& getUri() const;
    const std::string& getVersion() const;
    const std::string& getBody() const;
    const int& getStatusCode() const;

    bool isValidMethod() const;
    bool isValidVersion() const;
    bool isValidUri() const;
    bool hasHostHeader() const;
    void validate();


};

#endif