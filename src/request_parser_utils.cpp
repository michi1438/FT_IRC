/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_parser_utils.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzito <lzito@student.42lausanne.ch>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/21 16:18:54 by lzito             #+#    #+#             */
/*   Updated: 2024/04/23 13:38:16 by lzito            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/RequestParser.hpp"

void	readFromSocket(int client_socket, std::string &request)
{
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_read = 0;

        bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_read < 0)
		{
			std::cerr << "Error reading from socket" << std::endl;
			throw (500);
        }
		else if (bytes_read == 0)
		{
			std::cout << "Connection closed by client" << std::endl;
			throw (408);
		}

        request.append(buffer, bytes_read);
}

std::string getHttpRequest(int client_socket)
{
    std::string request;

    // Lire les données du socket jusqu'à ce que la connexion soit fermée par le client
    while (true)
	{
		readFromSocket(client_socket, request);

//        size_t header_end = request.find("\r\n\r\n");
//        if (header_end != std::string::npos)
//			break;
//		else
//			throw (400);

        size_t header_end = request.find("\r\n\r\n");
        if (header_end != std::string::npos)
		{
//            std::cout << "Header fully received" << std::endl;
            // Vérifier si la requête contient le Content-Length
            size_t content_length_start = request.find("Content-Length: ");
            if (content_length_start != std::string::npos)
			{
                size_t content_length_end = request.find("\r\n", content_length_start);
                if (content_length_end != std::string::npos)
				{
                    std::string content_length_str = request.substr(content_length_start + strlen("Content-Length: "), content_length_end - content_length_start - strlen("Content-Length: "));
                    int content_length = std::atoi(content_length_str.c_str());
                    // Vérifier si le corps de la requête est entièrement reçu
                    if (request.size() >= header_end + 4 + content_length)
					{
                        std::cout << "Request fully received" << std::endl;
                        break;
                    }
                }
            }
			else
			{
                std::cout << "Request fully received" << std::endl;
                break;
            }
		}
    }

//	std::string headers = request;
//	std::istringstream headers_stream(headers);
//	std::string new_line;
//	while (std::getline(headers_stream, new_line))
//	{
//		if (new_line.find("Content-Length: ") != std::string::npos)
//		{
//			int content_length = std::atoi(new_line.erase(new_line.size() - 1, 1).substr(16).c_str());
//			//TODO addBodyNotChunked(request, content_length);
//
//		}
//		else if (new_line.find("Transfer-Encoding: ") != std::string::npos)
//		{
//			if (new_line.find("chunked") != std::string::npos)
//				//TODO addBodyChunked(request);
//		}
//	}

//    std::cout << RED << request << RESET << std::endl;

    return request;
}
