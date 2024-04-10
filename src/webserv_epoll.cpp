/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv_epoll.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 09:41:31 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/10 12:40:32 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Centralinclude.hpp"
#include <sys/epoll.h>

std::string readHtmlFile(std::string filename, t_server srvr)
{
	
	std::fstream file;
	if (filename.empty() == false)
	{
		filename.insert(0, srvr.root);
		file.open(filename.c_str());
	}
	else
	{
		filename.append(srvr.root).append(srvr.home);
		file.open(filename.c_str());
	}
    if (!file.is_open())
	{
		std::ifstream file("html/40x.html");
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::string response = "HTTP/1.1 400 OK\r\nContent-Type: text/html\r\n\r\n" + content;
		file.close();
		return response;
    }
	else
	{
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + content;
		file.close();
		return response;
	}
}

int init_ws(ConfigFile& conf) 
{
    int ep = epoll_create1(0);
    if (ep == -1)
	{
        perror("Error in epoll"); // TODO instead send a err 500 page
        exit(EXIT_FAILURE);
    }
	
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
	{
        perror("Error in socket");
        exit(EXIT_FAILURE);
    }

	// while (prt_vec)
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

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
        perror("Error in epoll");
        exit(EXIT_FAILURE);
    }
	// end of while 
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

                // Add client socket to epoll
				change_event.events = EPOLLIN | EPOLLET;
				change_event.data.fd = client_fd;
                epoll_ctl(ep, EPOLL_CTL_ADD, client_fd, &change_event);
            }
			else
			{
				char buffer[4096];
                int client_socket = events[i].data.fd;
				
				// Lire la requête HTTP du client
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
				std::string host;
				std::getline(request_line_stream, method, ' ');
				std::getline(request_line_stream, uri, ' ');
				std::getline(request_stream, host);

				host = host.substr(host.find(' ') + 1);
				std::cout << "method : " <<  method << std::endl;
				std::cout << "uri : " << uri << std::endl;
				std::cout << "host : " << host << std::endl;

				// Vérifier si le chemin de l'URI correspond à un script CGI
				t_server srvr_used = choose_server(conf, host);
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
				else				{
					// Réponse normale (non-CGI)
				
					std::string response = readHtmlFile(uri.substr(1), srvr_used);

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
    close(ep);
    return 0;
}

t_server	 choose_server(const ConfigFile& conf, const std::string req_host)
{
	std::string host_name = req_host.substr(0, req_host.find(':'));
	int	host_port = atoi(req_host.substr(req_host.find(':') + 1).c_str());
	std::vector<t_server> servers = conf._map;
	for(std::vector<t_server>::const_iterator srvr_it = servers.begin(); srvr_it != servers.end(); srvr_it++)
	{
		for(std::vector<int>::const_iterator prtn_it = srvr_it->prtn.begin(); prtn_it != srvr_it->prtn.end(); prtn_it++)
		{
			if (*prtn_it == host_port)
			{
				for(std::vector<std::string>::const_iterator name_it = srvr_it->srvr_name.begin(); name_it != srvr_it->srvr_name.end(); name_it++)
				{
					if (*name_it == host_name)
					{
						std::cout << host_name << ":" << host_port << " was passed to (" << srvr_it->srvr_name[0] << ")server-block, for its name/port matched." << std::endl;
						return *srvr_it;
					}
				}
			}
		}
	}
	for(std::vector<t_server>::const_iterator srvr_it = servers.begin(); srvr_it != servers.end(); srvr_it++)
	{
		for(std::vector<int>::const_iterator prtn_it = srvr_it->prtn.begin(); prtn_it != srvr_it->prtn.end(); prtn_it++)
		{
			if (*prtn_it == host_port && srvr_it->is_default == true)
			{
				std::cout << host_name << ":" << host_port << " was passed to the default-tagged server-block (" << srvr_it->srvr_name[0] << ") with its port number." << std::endl;
				return *srvr_it;
			}
		}
	}
	t_server ret;
	ret.err = true;
	return ret;
}
