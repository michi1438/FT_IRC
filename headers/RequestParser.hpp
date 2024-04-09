/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzito <lzito@student.42lausanne.ch>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 11:45:09 by lzito             #+#    #+#             */
/*   Updated: 2024/04/09 09:23:15 by lzito            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <map>

# define MAX_HEADER_SIZE 4096
# define MAX_HEADER_FIELDS 100

class RequestParser
{
	private:
		std::map<std::string, std::string>	_header;
		std::map<std::string, std::string>	_query_param;
		std::string							_method;
		std::string							_uri;
		std::string							_host;
		std::string							_transfer_method;

		std::string							_body;
		std::string							_content_type;
		size_t								_content_length;

	public:
		RequestParser(const std::string &request);
		~RequestParser(void);

		bool		is_chunked();
		bool		is_CGI();

		std::string	getMethod() const;
		std::string	getURI() const;
		std::string	getHost() const;
		std::string	getTransferMethod() const;

		std::string getBody() const; //TODO tester si la requete est chunked et la unchunk avant de retourner le body si c'est le cas
		std::string getContentType() const;
		size_t		getContentLength() const;

		std::map<std::string, std::string>	getQueryParam() const;
};

#endif
