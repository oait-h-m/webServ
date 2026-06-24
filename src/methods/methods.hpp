#ifndef METHODS_HPP
#define METHODS_HPP

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "../request_parser/httpRequestParser.hpp"
#include "../request_handler/HttpResponse.hpp"

class Get
{
public:
    HttpResponse execute(const HttpRequest& request);
};

class Post
{
public:
    HttpResponse execute(const HttpRequest& request);
};

class Delete
{
public:
    HttpResponse execute(const HttpRequest& request);
};

bool isDirectory(const std::string &path);

#endif