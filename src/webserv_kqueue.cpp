/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv_kqueue.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzito <lzito@student.42lausanne.ch>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 14:55:00 by lzito             #+#    #+#             */
/*   Updated: 2024/04/04 15:56:46 by lzito            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Centralinclude.hpp"
#include <sys/event.h>

std::string readHtmlFile(const char *filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
	{
        return ""; // Ou une erreur appropriée
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return content;
}

int init_ws(ConfigFile& conf)
{
    int kq = kqueue();
    if (kq == -1)
	{
        perror("Error in kqueue");
        exit(EXIT_FAILURE);
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
	{
        perror("Error in socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(std::atoi(conf.getMap("prtn")));
	
	const int enable = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)); // TODO cleanup

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	{
        perror("Error in bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) == -1)
	{
        perror("Error in listen");
        exit(EXIT_FAILURE);
    }

    struct kevent events[MAX_EVENTS];
    struct kevent change_event;
    EV_SET(&change_event, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);

    if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1)
	{
        perror("Error in kevent");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started. Listening on port " << conf.getMap("prtn") << std::endl;

    while (true)
	{
        int num_events = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
        if (num_events == -1)
		{
            perror("Error in kevent");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < num_events; ++i)
		{
            if ((int)events[i].ident == server_fd)
			{
                // Accept new connection
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
                if (client_fd == -1)
				{
                    perror("Error in accept");
                    exit(EXIT_FAILURE);
                }

                // Set client socket to non-blocking
				if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
				{
                    perror("Error in fcntl");
                    exit(EXIT_FAILURE);
                }

				if (fcntl(client_fd, F_SETFL, FD_CLOEXEC) == -1)
				{
                    perror("Error in fcntl");
                    exit(EXIT_FAILURE);
                }

                std::cout << "New connection accepted" << std::endl;

                // Add client socket to kqueue
                EV_SET(&change_event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1)
				{
                    perror("Error in kevent");
                    exit(EXIT_FAILURE);
                }
            }
			else
			{
                int client_socket = events[i].ident;

				// Lire la requête HTTP du client
				char buffer[4096];
				int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
				if (bytes_received <= 0)
				{
					perror("Error receiving request");
					close(client_socket);
					continue;
				}
				std::cout << buffer << std::endl;

				// Analyser la requête HTTP pour extraire le chemin de l'URI
				std::string request(buffer, buffer + bytes_received);
				std::istringstream request_stream(request);
				std::string request_line;
				std::getline(request_stream, request_line);

				std::istringstream request_line_stream(request_line);
				std::string method;
				std::string uri;
				std::getline(request_line_stream, method, ' ');
				std::getline(request_line_stream, uri, ' ');

				std::cout << method << std::endl;
				std::cout << uri << std::endl;

				// Vérifier si le chemin de l'URI correspond à un script CGI
				if (uri.find("/cgi_bin/") == 0)
				{
					// Exécuter le script CGI
					std::string cgi_script_path = "." + uri;
					std::string cgi_output = "<h1>CGI handling</h1>";//execute_cgi_script(cgi_script_path);

					// Envoyer la sortie du script CGI au client
					std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + cgi_output;
					if (send(client_socket, response.c_str(), response.size(), 0) == -1)
					{
						perror("Error in send");
						close(client_socket);
						continue;
					}
					close(client_socket);
					std::cout << BLUE << "Response sent from CGI" << RESET << std::endl;
				}
				else
				{
					// Réponse normale (non-CGI)
					std::string htmlContent = readHtmlFile(uri.substr(1).c_str());
					std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + htmlContent;

					if (send(client_socket, response.c_str(), response.size(), 0) == -1)
					{
						perror("Error in send");
						close(client_socket);
						return 1;
					}

					close(client_socket);
					std::cout << BLUE << "Response sent." << RESET << std::endl;
				}
            }
        }
    }
    close(server_fd);
    close(kq);
    return 0;
}
