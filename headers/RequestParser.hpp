/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgodtsch <rgodtsch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 11:45:09 by lzito             #+#    #+#             */
/*   Updated: 2024/05/09 13:27:50 by rgodtsch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

#include <iostream>
#include <iomanip>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <map>

# define MAX_HEADER_SIZE 4096
# define BUFFER_SIZE 4096
# define MAX_HEADER_FIELDS 100

# define RESET   "\033[0m"
# define RED     "\033[31m"
# define GREEN   "\033[32m"
# define YELLOW  "\033[33m"
# define BLUE    "\033[34m"
# define MAGENTA "\033[35m"
# define CYAN    "\033[36m"

class RequestParser
{
	private:
		std::string							_method;
		std::string							_uri;
		std::string							_version;
		std::string							_host;
		std::string							_script_name;

		bool								_is_chunked;
		std::string							_boundary;
		std::string							_content_type;
		size_t								_content_length;
		std::string							_body;

		std::map<std::string, std::string>	_query_param;

	public:
		RequestParser(void);
		RequestParser(const int &client_socket);
		~RequestParser(void);

		void		show() const;
		std::string unchunk(std::string body) const;

		bool		isChunked() const;
		bool		isCGI() const;

		std::string	getMethod() const;
		std::string	getURI() const;
		std::string	getVersion() const;
		std::string	getHost() const;
		std::string	getScriptName() const;

		std::string getBody() const;
		std::string getContentType() const;
		std::string	getBoundary() const;
		std::string getQueryString() const;
		size_t		getContentLength() const;

		std::map<std::string, std::string>	getQueryParam() const;
		std::string toString() const;
		static RequestParser fromString(const std::string &req_str);
};

std::string getHttpRequest(int client_socket);

#endif
