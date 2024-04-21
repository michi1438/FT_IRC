/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv_epoll.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 09:41:31 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/21 12:02:45 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Centralinclude.hpp"
#include <sys/epoll.h>

std::string readHtmlFile(std::string filename, t_server srvr, bool err_50x)
{
	if (err_50x == true)
	{
		std::ifstream file(ERR_500);
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::string response = "HTTP/1.1 500 OK\r\nContent-Type: text/html\r\n\r\n" + content;
		file.close();
		return response;
	}
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

int prts_is_open(std::vector<int> server_fd, int fd)
{
	for(std::vector<int>::iterator it = server_fd.begin(); it != server_fd.end(); it++)
	{
		if (*it == fd)
			return *it;
	}
	return -1;
}

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
        perror("Error in epoll"); // TODO instead send a err 500 page
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

				try
				{
					RequestParser Req(buffer);

					// Vérifier si le chemin de l'URI correspond à un script CGI
					t_server srvr_used = choose_server(conf, Req.getHost());
					std::cout << buffer << std::endl;
					std::cout << std::endl;
					Req.show();

					// Vérifier si le chemin de l'URI correspond à un script CGI
					if (Req.getMethod() == "POST" && Req.getScriptName() == "upload") 
					{
						handleFileUpload(Req);
						std::string response = readHtmlFile("./html/upload.html", srvr_used, false);
						send(client_socket, response.c_str(), response.size(), 0);
						close(client_socket);
						std::cout << BLUE << "Response upload sent." << RESET << std::endl;
					}
					if (Req.getVersion().compare(HTTP_VER) != 0)
					{
						// ERREUR 500
						std::string response = readHtmlFile(Req.getURI().substr(1), srvr_used, true);
						send(client_socket, response.c_str(), response.size(), 0);
						close(client_socket);
						std::cout << BLUE << "Response 500 sent." << RESET << std::endl;
					}
					else if (Req.isCGI())
					{
						//TODO Exécuter le script CGI dans un nouveau process
						std::string cgi_script_path = "." + Req.getURI();
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
						//TODO clear le buffer, sinon il garde des infos des requetes precedentes
						std::cout << BLUE << "Response sent from CGI" << RESET << std::endl;
					}
					else
					{
						// Réponse normale (non-CGI)
						std::string response = readHtmlFile(Req.getURI().substr(1).c_str(), srvr_used, false);

						if (send(client_socket, response.c_str(), response.size(), 0) == -1)
						{
							perror("Error in send");
							close(client_socket);
							return 1;
						}
						close(client_socket);
						//TODO clear le buffer, sinon il garde des infos des requetes precedentes
						std::cout << BLUE << "Response sent." << RESET << std::endl;
					}
				}
				catch (int errorCode)
				{
					//TODO Afficher la bonne page html selon le code d'erreur
					// switch case ?
	 				std::cout << RED << "ERROR CODE : " << errorCode << std::endl;
					std::cout << RESET;
					close(client_socket);
					//TODO clear le buffer, sinon il garde des infos des requetes precedentes
					// faire ca plus proprement que comme ca :
					char *begin = buffer;
					char *end = begin + sizeof(buffer);
					std::fill(begin, end, 0);
				}
			}
        }
    }
	for(std::vector<int>::iterator it = prt_vec.begin(); it != prt_vec.end(); it++)
		close(*it);
    close(ep);
    return 0;
}

t_server	 choose_server(const ConfigFile& conf, const std::string req_host)
{
	std::string host_name = req_host.substr(0, req_host.find(':'));
	int	host_port = atoi(req_host.substr(req_host.find(':') + 1).c_str());
	std::vector<t_server> servers = conf.getBlocks();
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
