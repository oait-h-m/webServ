#include "HttpResponse.hpp"

HttpResponse::HttpResponse(): status_code(200)
{
    statusMessages[200] = "OK";
    statusMessages[201] = "Created";
    statusMessages[204] = "No Content";
    statusMessages[301] = "Moved Permanently";
    statusMessages[302] = "Found";
    statusMessages[400] = "Bad Request";
    statusMessages[403] = "Forbidden";
    statusMessages[404] = "Not Found";
    statusMessages[405] = "Method Not Allowed";
    statusMessages[413] = "Payload Too Large";
    statusMessages[500] = "Internal Server Error";
    statusMessages[501] = "Not Implemented";
    statusMessages[502] = "Bad Gateway";
    statusMessages[504] = "Gateway Timeout";
    statusMessages[505] = "HTTP Version Not Supported";

    mime[".html"] = "text/html";
    mime[".css"]  = "text/css";
    mime[".js"]   = "application/javascript";
    mime[".txt"]  = "text/plain";
    mime[".jpg"]  = "image/jpeg";
    mime[".jpeg"] = "image/jpeg";
    mime[".png"]  = "image/png";
    mime[".gif"]  = "image/gif";
    mime[".bmp"]  = "image/bmp";
    mime[".ico"]  = "image/x-icon";
    mime[".svg"]  = "image/svg+xml";
    mime[".webp"] = "image/webp";
    mime[".pdf"]  = "application/pdf";
    mime[".json"] = "application/json";
}

HttpResponse::~HttpResponse()
{}
    
void HttpResponse::setStatus(int code)
{
    status_code = code;
    status_message = statusMessages[status_code];
}

void HttpResponse::setBody(const std::string &content)
{
    body = content;
    std::ostringstream ss;
    ss << body.length();
    setHeader("Content-Length", ss.str());
}

void HttpResponse::setHeader(const std::string &key, const std::string &value)
{
    headers[key] = value;
}

std::string HttpResponse::build()
{
    std::ostringstream response;
    
    response << "HTTP/1.1 " << status_code << " " << status_message << "\r\n";
    for (std::map<std::string, std::string>::iterator it = headers.begin(); 
         it != headers.end(); ++it) {
        response << it->first << ": " << it->second << "\r\n";
    }
    response << "\r\n";
    response << body;
    
    return response.str();
}

std::string HttpResponse::getMime(const std::string& path)
{
    size_t pos = path.find_last_of('.');

    if (pos == std::string::npos)
        return "application/octet-stream";
    std::string key = path.substr(pos);
    std::map<std::string, std::string>::iterator it = mime.find(key);
    if (it != mime.end())
        return it->second;

    return "application/octet-stream";
}

int HttpResponse::getStatusCode() 
{
    return status_code;
}

std::string HttpResponse::getStatusMessage() 
{
    return status_message;
}

std::string HttpResponse::getBody() 
{
    return body;
}

std::string HttpResponse::getHeader(const std::string &key) 
{
    return headers[key];
}