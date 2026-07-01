#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include "../request_parser/httpRequestParser.hpp"
# include "../config_file_parser/InternalConfigs.hpp"

class CgiHandler {
	private:
		void _validate_path();
		std::string _resolve_script_path();
		LocationConfig &lc_config;
		HttpRequest &request;
		CgiHandler();

	public:
		CgiHandler(LocationConfig &config, HttpRequest &request);
		CgiHandler(const CgiHandler &src);
		~CgiHandler();
		CgiHandler& operator=(const CgiHandler &src);
		static bool is_cgi_request(LocationConfig &config, HttpRequest &request);
		std::string execute();
};

#endif /* CGIHANDLER_HPP */
