#ifndef POST_HPP
#define POST_HPP

#include "../request_parser/httpRequestParser.hpp"
#include "../request_handler/HttpResponse.hpp"

class Post
{
public:
    HttpResponse execute(const HttpRequest& request);
};

#endif