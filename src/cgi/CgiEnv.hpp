#ifndef CGIENV_HPP
# define CGIENV_HPP

#include "../config_file_parser/InternalConfigs.hpp"
#include "../request_parser/httpRequestParser.hpp"

class CgiEnv {
	private:
		char **_envp; /* The container for Environment variables */
		LocationConfig &_lc_conf;
		HttpRequest		&_request;
		CgiEnv();
		CgiEnv(const CgiEnv &other);
		CgiEnv &operator=(const CgiEnv &other);
		void	_build_env();
	public:
		CgiEnv(LocationConfig &conf, HttpRequest &req);
		~CgiEnv();
		char **get_envp() const;

};

#endif /* CGIENV_HPP */
