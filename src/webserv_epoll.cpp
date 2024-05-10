/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv_epoll.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 09:41:31 by mguerga           #+#    #+#             */
/*   Updated: 2024/05/22 15:35:59 by robin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Centralinclude.hpp"
#include <sys/epoll.h>

int init_ws(ConfigFile& conf) 
{
    int ep = epoll_create1(0);
	struct epoll_event events[MAX_EVENTS];
	struct epoll_event change_event;
	std::vector<int> prt_vec = conf.getPort_vec(); 
	const int enable = 1;
	change_event.events = EPOLLIN;
    if (ep == -1)
	{
        perror("Error in epoll");
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
		change_event.data.fd = server_fd.back();
		if (epoll_ctl(ep, EPOLL_CTL_ADD, server_fd.back(), &change_event) == -1)
		{
			perror("Error in epoll");
			exit(EXIT_FAILURE);
		}

	}
    std::cout << "Webserv started. Listening on port(s) " << conf.prt_vec_print() << std::endl;

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
			int cur_srv_fd;
			if ((cur_srv_fd = prts_is_open(server_fd, (int)events[i].data.fd)) != -1)
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

				// Set client socket to non-blocking and close on exec
				if (fcntl(client_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
				{
					perror("Error in fcntl");
					exit(EXIT_FAILURE);
				}

				if (fcntl(client_fd, F_SETFD, FD_CLOEXEC) == -1)
				{
					perror("Error in fcntl");
					exit(EXIT_FAILURE);
				}
				//std::cout << "GETFL " << fcntl(client_fd, F_GETFL, FD_CLOEXEC) << std::endl; TODO check the nonblock and cloexec is on SETFD or SETFL
				std::cout << "New connection accepted" << std::endl;

				// Add client socket to epoll
				change_event.events = EPOLLIN | EPOLLET;
				change_event.data.fd = client_fd;
				if (epoll_ctl(ep, EPOLL_CTL_ADD, client_fd, &change_event) == -1)
				{
                    perror("Error in epoll");
                    exit(EXIT_FAILURE);
				}
			}
			else
			{
				int client_socket = events[i].data.fd;

				RequestParser Req;
				t_server srvr_used;
				try
				{
					RequestParser R(client_socket, conf);
					Req = R;
					srvr_used = choose_server(conf, Req.getHost());
					requestHandler(client_socket, &srvr_used, Req);
				}
				catch (int errorCode)
				{
	 				std::cout << RED << "ERROR CODE : " << errorCode << RESET << std::endl;
					std::string response = read_errpage(errorCode, Req, srvr_used);
					int bytes_sent = send(client_socket, response.c_str(), response.size(), 0);
					if (bytes_sent == 0)
					{
						std::cout << "Zero bytes were sent, this ain't normal" << std::endl; // TODO find better message...
						throw (500);
					}
					if (bytes_sent == -1)
						throw (501);
					close(client_socket);
				}
			}
        }
    }
	for(std::vector<int>::iterator it = prt_vec.begin(); it != prt_vec.end(); it++)
		close(*it);
    close(ep);
    return 0;
}
