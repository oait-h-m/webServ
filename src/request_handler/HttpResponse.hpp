#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>
#include <sstream>

class HttpResponse {
private:
    int status_code;
    std::string status_message;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> mime_type;
    std::string body;
    std::map<int, std::string> statusMessages;
    std::map<std::string, std::string> mime;
    
public:
    HttpResponse();
    ~HttpResponse();
    
    void setStatus(int code);
    void setBody(const std::string &content);
    void setHeader(const std::string &key, const std::string &value);
    
    std::string build();
    
    int getStatusCode() ;
    std::string getStatusMessage() ;
    std::string getMime(const std::string& path) ;
    std::string getBody() ;
    std::string getHeader(const std::string &key) ;
};

#endif