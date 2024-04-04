/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv_epoll.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 09:41:31 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/04 15:56:37 by lzito            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Centralinclude.hpp"
#include <sys/epoll.h>

std::string readHtmlFile(const char *filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
	{
		std::ifstream file("ERR500/50x.html");
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();
		return content;
    }
	else
	{
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();
		return content;
	}
}

int init_ws(ConfigFile& conf) 
{
    int ep = epoll_create1(0);
    if (ep == -1)
	{
        perror("Error in epoll");
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

	struct epoll_event events[MAX_EVENTS];
	struct epoll_event change_event;
	change_event.events = EPOLLIN;
	change_event.data.fd = server_fd;
	if (epoll_ctl(ep, EPOLL_CTL_ADD, server_fd, &change_event) == -1)
	{
        perror("Error in kevent");
        exit(EXIT_FAILURE);
    }
    std::cout << "Server started. Listening on port " << conf.getMap("prtn") << std::endl;

    while (true)
	{
        int num_events = epoll_wait(ep, events, MAX_EVENTS, -1);
        if (num_events == -1)
		{
            perror("Error in epoll");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < num_events; ++i)
		{
            if ((int)events[i].data.fd == server_fd)
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
				change_event.events = EPOLLIN | EPOLLET;
				change_event.data.fd = client_fd;
                epoll_ctl(ep, EPOLL_CTL_ADD, client_fd, &change_event);
            }
			else
			{
				char buffer[4096];
                int client_socket = events[i].data.fd;
				
				recv(client_socket, buffer, sizeof(buffer), 0);
				// Réponse HTTP avec le contenu de socket.html
				std::string htmlContent = readHtmlFile("index.html");
				std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + htmlContent;

				if (send(client_socket, response.c_str(), response.size(), 0) == -1)
				{
					perror("Error in send");
					return 1;
				}
				close(client_socket);
				std::cout << BLUE << "Response sent." << RESET << std::endl;
            }
        }
    }
    close(server_fd);
    close(ep);
    return 0;
}
