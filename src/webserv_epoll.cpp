/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv_epoll.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 09:41:31 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/25 09:39:50 by lzito            ###   ########.fr       */
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
				epoll_ctl(ep, EPOLL_CTL_ADD, client_fd, &change_event);
			}
			else
			{
				// Lire la requête HTTP du client
				int client_socket = events[i].data.fd;

				try
				{
					RequestParser Req(client_socket);

					t_server srvr_used = choose_server(conf, Req.getHost());
					if (Req.getVersion().compare(HTTP_VER) != 0)
						throw (505);
					if (srvr_used.method.compare("ALL") != 0 && srvr_used.method.find("." + Req.getMethod() + " ") == std::string::npos)
						throw (405);						

					Req.show();
					
					if (Req.getMethod() == "POST" && Req.getScriptName() == "upload") 
					{
						handleFileUpload(Req);
						showUploadedFiles(client_socket);
						close(client_socket);
						std::cout << BLUE << "Response upload sent." << RESET << std::endl;
						continue;
					}
					else if(Req.getMethod() == "GET" && Req.getURI().find("/upload/") != std::string::npos && !Req.isCGI()){
						std::string filename = Req.getURI().substr(8);
						handleFileDownload(Req, client_socket, filename);
					}
					else if(Req.getMethod() == "POST" && Req.getURI().find("/delete") != std::string::npos && !Req.isCGI()){
						std::string body = Req.getBody();
						std::string filename = body.find("file_to_delete=") != std::string::npos ? body.substr(15) : "";
						handleFileDelete(filename, client_socket);
					}

					// Vérifier si le chemin de l'URI correspond à un script CGI
					else if (Req.isCGI())
					{
						// Exécuter le script CGI
						std::string cgi_script_path = "cgi_bin/" + Req.getScriptName();
						//std::string cgi_output = "<h1>CGI handling</h1>";//execute_cgi_script(cgi_script_path);
                        std::string cgi_output = execute_cgi_script(cgi_script_path, Req);
						// Envoyer la sortie du script CGI au client
						std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + cgi_output;
						if (send(client_socket, response.c_str(), response.size(), 0) == -1)
							throw (501);
						close(client_socket);
						std::cout << BLUE << "Response sent from CGI" << RESET << std::endl;
					}
					else
					{
						// Réponse normale (non-CGI)
						std::string response = readHtmlFile(Req.getURI().substr(1).c_str(), srvr_used);

						if (send(client_socket, response.c_str(), response.size(), 0) == -1)
							throw (501);
						close(client_socket);
						std::cout << BLUE << "Response sent." << RESET << std::endl;
					}
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
    close(ep);
    return 0;
}
