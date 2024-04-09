/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzito <lzito@student.42lausanne.ch>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 11:45:34 by lzito             #+#    #+#             */
/*   Updated: 2024/04/09 09:46:00 by lzito            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/RequestParser.hpp"

RequestParser::RequestParser(const std::string &request)
{
	//TODO parcourir la requete et populer les variables
	// First line (Method & uri)
	std::istringstream	request_stream(request);
	std::string			first_line;

	std::getline(request_stream, first_line);
	std::istringstream	first_line_stream(first_line);

	std::getline(first_line_stream, this->_method, ' ');
	std::getline(first_line_stream, this->_uri, ' ');

	// Second line (Host)
	std::string			second_line;
	std::getline(request_stream, second_line);
	this->_host = second_line.substr(6);

	std::string			new_line;
	std::getline(request_stream, new_line);
//	while (new_line.find("\n") == std::string::npos)
//	{
//		std::cout << "BLA" << std::endl;
//	}
}

RequestParser::~RequestParser()
{
	return ;
}

std::string RequestParser::getMethod() const
{
	return (this->_method);
}

std::string RequestParser::getURI() const
{
	return (this->_uri);
}

std::string RequestParser::getHost() const
{
	return (this->_host);
}

std::string RequestParser::getTransferMethod() const
{
	return (this->_transfer_method);
}

std::string RequestParser::getBody() const
{
	return (this->_body);
}

std::string RequestParser::getContentType() const
{
	return (this->_content_type);
}

size_t RequestParser::getContentLength() const
{
	return (this->_content_length);
}

std::map<std::string, std::string> RequestParser::getQueryParam() const
{
	return (this->_query_param);
}
