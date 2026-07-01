#include "./CgiHandler.hpp"

CgiHandler::CgiHandler(LocationConfig &conf, HttpRequest &req):
	lc_config(conf), request(req) {}

CgiHandler::CgiHandler(const CgiHandler &other): 
	lc_config(other.lc_config), request(other.request) {}

CgiHandler& CgiHandler::operator=(const CgiHandler &other) {
	return (*this);
}

CgiHandler::~CgiHandler() {}

bool CgiHandler::is_cgi_request(LocationConfig &conf, HttpRequest &req) {

	if (conf.cgi_extension.empty() == true)
		return (false);
	if (req.getMethod() == "DELETE")
		return (false);

	std::string file_path;
	std::string uri = req.getUri();
	std::string extension;
	std::string::size_type pos = uri.find('?');
	if (pos == std::string::npos)
		file_path = uri;
	else
		file_path = uri.substr(0, pos);
	pos = file_path.rfind('/');
	if (pos != std::string::npos)
		file_path = file_path.substr(pos);
	pos = file_path.rfind('.');
	if (pos == std::string::npos)
		return (false);
	extension = file_path.substr(pos);
	if (extension == conf.cgi_extension)
		return (true);
	return (false);
}

std::string CgiHandler::execute() {
	std::string prod;
	return (prod);
}

void CgiHandler::_validate_path() {}
std::string CgiHandler::_resolve_script_path() {
	std::string prod;
	return (prod);
}
