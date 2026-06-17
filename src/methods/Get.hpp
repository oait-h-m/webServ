#ifndef GET_HPP
#define GET_HPP

#include "../request_parser/httpRequestParser.hpp"
#include "../request_handler/HttpResponse.hpp"

class Get
{
public:
    HttpResponse execute(const HttpRequest& request);
};

#endif