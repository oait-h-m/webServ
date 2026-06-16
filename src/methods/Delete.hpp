#ifndef DELETE_HPP
#define DELETE_HPP

#include "../request_parser/httpRequestParser.hpp"
#include "../request_handler/HttpResponse.hpp"

class Delete
{
public:
    HttpResponse execute(const HttpRequest& request);
};

#endif