/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzito <lzito@student.42lausanne.ch>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 11:45:34 by lzito             #+#    #+#             */
/*   Updated: 2024/04/11 14:49:45 by lzito            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/RequestParser.hpp"

RequestParser::RequestParser(const std::string &request)
	: _method(""), _uri(""), _host(""), _boundary(""),
	_content_type(""), _content_length(0), _body("")
{
	// First line (Method & URI)
	std::istringstream	request_stream(request);

	std::string			first_line;
	std::getline(request_stream, first_line);

	std::istringstream	first_line_stream(first_line);
	std::getline(first_line_stream, this->_method, ' ');
	std::getline(first_line_stream, this->_uri, ' ');

	// Second line (Host)
	std::string			second_line;
	std::getline(request_stream, second_line, '\r');
	this->_host = second_line.substr(6);

	if (this->_method == "GET")
		return ;

	// Rest of headers
	std::string			new_line;
	std::getline(request_stream, new_line);
	while (new_line.size() != 1)
	{
		if (new_line.find("Content-Type: multipart/form-data") != std::string::npos)
		{
			std::istringstream new_line_stream(new_line);
			std::getline(new_line_stream, this->_content_type, ' ');
			std::getline(new_line_stream, this->_content_type, ';');
			std::getline(new_line_stream, this->_boundary, '=');
			std::getline(new_line_stream, this->_boundary, '\r');
		}
		else if (new_line.find("Content-Type: ") != std::string::npos)
			this->_content_type = new_line.erase(new_line.size() - 1, 1).substr(14);
		else if (new_line.find("Content-Length: ") != std::string::npos)
		{
			this->_content_length = std::atoi(new_line.erase(new_line.size() - 1, 1).substr(16).c_str());
			if (this->_content_length > 500) //TODO get body limit from ConfigFile
				throw 413;
		}
		std::getline(request_stream, new_line);
	}

	// body
	for (std::string line; std::getline(request_stream, line);)
	{
		line.append("\n");
		this->_body.append(line);
	}
}

RequestParser::~RequestParser()
{
	return ;
}

void RequestParser::show() const
{
	std::cout << GREEN;
	std::cout << std::setw(50) << "----------------------------------------" << std::endl;
	std::cout << std::setw(50) << "|            REQUEST PARSER            |" << std::endl;
	std::cout << std::setw(50) << "----------------------------------------" << std::endl;
	std::cout << RESET << std::setw(20) << "METHOD : " << GREEN << this->getMethod() << "$" << std::endl;
	std::cout << RESET << std::setw(20) << "URI : " << GREEN << this->getURI() << "$" << std::endl;
	std::cout << RESET << std::setw(20) << "HOST : " << GREEN << this->getHost() << "$" << std::endl;
	std::cout << RESET << std::setw(20) << "BOUNDARY : " << GREEN << this->getBoundary() << "$" << std::endl;
	std::cout << RESET << std::setw(20) << "CONTENT TYPE : " << GREEN << this->getContentType() << "$" << std::endl;
	std::cout << RESET << std::setw(20) << "CONTENT LENGTH : " << GREEN << this->getContentLength() << "$" << std::endl;
	std::cout << RESET << std::setw(20) << "BODY : " << std::endl;
	std::cout << CYAN << this->getBody() << "$" << std::endl;
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

std::string RequestParser::getBoundary() const
{
	return (this->_boundary);
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
