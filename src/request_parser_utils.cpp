/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_parser_utils.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/21 16:18:54 by lzito             #+#    #+#             */
/*   Updated: 2024/05/22 17:17:46 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/RequestParser.hpp"

std::string decodeUri(const std::string &uri)
{
	std::stringstream decoded;
	decoded << std::hex << std::setfill('0');
	for (size_t i = 0; i < uri.length(); ++i)
	{
        if (uri[i] == '%')
		{
            if (i + 2 < uri.length())
			{
                int value;
                std::istringstream(uri.substr(i + 1, 2)) >> std::hex >> value;
                decoded << static_cast<char>(value);
                i += 2;
            }
        }
		else if (uri[i] == '+')
            decoded << ' ';
		else
			decoded << uri[i];
    }
    return (decoded.str());
}

void	readFromSocket(int client_socket, std::string &request)
{
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_read = 0;

        bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_read < 0)
			return ;
		else if (bytes_read == 0)
		{
			std::cout << "Connection closed by client" << std::endl;
			throw (408);
		}

        request.append(buffer, bytes_read);
}

void	addBody(int client_socket, std::string &request, int content_length)
{
	if ((int)request.size() >= 4 + content_length)
		return ;
    while (true)
	{
		readFromSocket(client_socket, request);
		// std::cout << "client_socket : " << client_socket << std::endl;
		// std::cout << "content length : " << content_length << std::endl;
		// std::cout << "request size : " << request.size() << std::endl;
		// std::cout << "request : " << request << std::endl;

		// Vérifier si le corps de la requête est entièrement reçu
		if ((int)request.size() >= 4 + content_length)
		{
			std::cout << "Request fully received 1" << std::endl;
			break;
		}
	}
}

int	checkSizes(const std::string &request, const ConfigFile &conf)
{
	std::string headers = request;
	std::istringstream headers_stream(headers);

	// First line (Method, URI & Version)
	///////////////////////////////////////
	std::string	method;
	std::string	uri;
	std::string	version;
	std::string	host;
	int content_length = 0;

	std::string	first_line;
	std::getline(headers_stream, first_line);
	if (first_line.size() >= MAX_HEADER_SIZE)
		throw (414);

	std::istringstream	first_line_stream(first_line);
	std::getline(first_line_stream, method, ' ');
	std::getline(first_line_stream, uri, ' ');
	std::getline(first_line_stream, version, '\r');

	// Second line (Host)
	///////////////////////////////////////
	std::string	second_line;
	std::getline(headers_stream, second_line);
	host = second_line.erase(second_line.size() - 1, 1).substr(6);

	std::string	new_line;
	std::getline(headers_stream, new_line);
	while (true)
	{
		if (new_line.find("Content-Length: ") != std::string::npos)
		{
			t_server srvr_used = choose_server(conf, host);
			if (!srvr_used.locations.empty())
				srvr_used = update_location(srvr_used, uri);
			std::stringstream ss(new_line.erase(new_line.size() - 1, 1).substr(16));
			ss >> content_length;
			if (content_length > srvr_used.lcbs)
				throw (413);
		}
		else if (new_line == "\r")
			break;
		std::getline(headers_stream, new_line);
	}
	return (content_length);
}

std::string getHttpRequest(int client_socket, const ConfigFile &conf)
{
    std::string request;
	size_t header_end = 0;

    while (true)
	{
		readFromSocket(client_socket, request);

        header_end = request.find("\r\n\r\n");
        if (header_end != std::string::npos)
			break;
    }

	int content_length = checkSizes(request, conf);
	addBody(client_socket, request, content_length + header_end);

    return request;
}
