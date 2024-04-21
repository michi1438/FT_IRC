/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_parser_utils.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzito <lzito@student.42lausanne.ch>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/21 16:18:54 by lzito             #+#    #+#             */
/*   Updated: 2024/04/21 17:17:58 by lzito            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/RequestParser.hpp"

std::string getHttpRequest(int client_socket)
{
    std::string request;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Lire les données du socket jusqu'à ce que la connexion soit fermée par le client
    while (true)
	{
        bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_read < 0)
		{
			std::cerr << "Error reading from socket" << std::endl;
			return "";
        }
		else if (bytes_read == 0)
		{
			std::cout << "Connection closed by client" << std::endl;
			break;
		}

        request.append(buffer, bytes_read);

        // Si la requête contient la séquence de fin "\r\n\r\n", cela signifie que la requête est complète
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
                    std::string content_length_str = request.substr(content_length_start + 16, content_length_end - content_length_start - 16);
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

//    std::cout << RED << request << RESET << std::endl;

    return request;
}
