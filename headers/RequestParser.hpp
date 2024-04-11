/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzito <lzito@student.42lausanne.ch>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 11:45:09 by lzito             #+#    #+#             */
/*   Updated: 2024/04/11 15:03:08 by lzito            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

# define MAX_HEADER_SIZE 4096
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
		std::map<std::string, std::string>	_header; //TODO needed ?
		std::map<std::string, std::string>	_query_param;
		std::string							_method;
		std::string							_uri;
		std::string							_host;

		std::string							_boundary;
		std::string							_content_type;
		size_t								_content_length;
		std::string							_body;

	public:
		RequestParser(const std::string &request);
		~RequestParser(void);

		bool		is_chunked();
		bool		is_CGI();

		void		show() const;
		std::string	getMethod() const;
		std::string	getURI() const;
		std::string	getHost() const;

		std::string getBody() const; //TODO tester si la requete est chunked et la unchunk avant de retourner le body si c'est le cas
		std::string getContentType() const;
		std::string	getBoundary() const;
		size_t		getContentLength() const;

		std::map<std::string, std::string>	getQueryParam() const; //TODO
};

#endif
