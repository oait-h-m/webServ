#include "methods.hpp"

bool isDirectory(const std::string &path)
{
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0)
    {
        return false;
    }
    return S_ISDIR(buffer.st_mode);
}

HttpResponse Get::execute(const HttpRequest& request)
{
    HttpResponse response;
    std::string uri = request.getUri();
    if (uri == "/")
        uri = "/index.html";

    if (uri.find("..") != std::string::npos)
    {
        response.setStatus(403);
        response.setBody("403 Forbidden");
        return response;
    }

    std::string path = "./www" + uri;
    if (isDirectory(path))
    {
        response.setStatus(403);
        response.setBody("403 Forbidden");
        return response;
    }

    std::ifstream file(path.c_str());
    if (!file.is_open())
    {
        response.setBody("404 File Not Found");
        response.setStatus(404);
        return response;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileContent = buffer.str();

    response.setStatus(200);
    response.setBody(fileContent);
    response.setHeader("Content-Type", response.getMime(path));

    return response;
}