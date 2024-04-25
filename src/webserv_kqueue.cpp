/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv_kqueue.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 14:55:00 by lzito             #+#    #+#             */
/*   Updated: 2024/04/25 10:23:54 by lzito            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Centralinclude.hpp"
#include <sys/event.h>

int init_ws(ConfigFile& conf)
{
    int kq = kqueue();
	const int enable = 1;
    struct kevent events[MAX_EVENTS];
    struct kevent change_event;
	std::vector<int> prt_vec = conf.getPort_vec(); 
    if (kq == -1)
    {
        perror("Error in kqueue");
        exit(EXIT_FAILURE);
    }

	std::vector<int> server_fd; 
	for(std::vector<int>::iterator it = prt_vec.begin(); it != prt_vec.end(); it++)
	{		
		server_fd.push_back(socket(AF_INET, SOCK_STREAM, 0));
		if (server_fd.back() == -1)
		{
			perror("Error in socket");
			exit(EXIT_FAILURE);
		}

		struct sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = INADDR_ANY;
		server_addr.sin_port = htons(*it);
		setsockopt(server_fd.back(), SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)); // TODO cleanup
		if (bind(server_fd.back(), (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		{
			perror("Error in bind");
			exit(EXIT_FAILURE);
		}

		if (listen(server_fd.back(), 10) == -1)
		{
			perror("Error in listen");
			exit(EXIT_FAILURE);
		}

		EV_SET(&change_event, server_fd.back(), EVFILT_READ, EV_ADD, 0, 0, NULL);

		if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1)
		{
			perror("Error in kevent");
			exit(EXIT_FAILURE);
		}
	}
    std::cout << "Webserv started. Listening on port(s) " << conf.prt_vec_print() << std::endl;

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
			int cur_srv_fd;
			if ((cur_srv_fd = prts_is_open(server_fd, (int)events[i].ident)) != -1)
			{
                // Accept new connection
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(cur_srv_fd, (struct sockaddr*)&client_addr, &client_len);
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

				try
				{
					requestHandler(client_socket, conf);
				}
				catch (int errorCode)
				{
	 				std::cout << RED << "ERROR CODE : " << errorCode << RESET << std::endl;
					std::string response = read_errpage(errorCode);
					send(client_socket, response.c_str(), response.size(), 0);
					close(client_socket);
				}
            }
        }
    }
	for(std::vector<int>::iterator it = prt_vec.begin(); it != prt_vec.end(); it++)
		close(*it);
    close(kq);
    return 0;
}
