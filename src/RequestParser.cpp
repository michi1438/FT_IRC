/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzito <lzito@student.42lausanne.ch>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 11:45:34 by lzito             #+#    #+#             */
/*   Updated: 2024/04/05 15:26:21 by lzito            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/RequestParser.hpp"

RequestParser::RequestParser(const std::string &request)
{
	//TODO parcourir la requete et populer les variables
	std::istringstream	request_stream(request);
	std::string			first_line;

	std::getline(request_stream, first_line);
	std::istringstream	first_line_stream(first_line);

	std::getline(first_line_stream, this->_method, ' ');
	std::getline(first_line_stream, this->_uri, ' ');
	std::getline(first_line_stream, this->_version, ' ');
}

RequestParser::~RequestParser()
{
	return ;
}

std::string RequestParser::getURI() const
{
	return (this->_uri);
}

std::string RequestParser::getMethod() const
{
	return (this->_method);
}

std::string RequestParser::getVersion() const
{
	return (this->_version);
}
