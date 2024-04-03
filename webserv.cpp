/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 11:15:52 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/03 13:32:34 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Centralinclude.hpp"

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
	(void)conf;
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

    if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
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

				// Réponse HTTP avec le contenu de socket.html
				std::string htmlContent = readHtmlFile("index.html");
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
    close(server_fd);
    close(kq);
    return 0;
}
