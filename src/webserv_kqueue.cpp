/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv_kqueue.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 14:55:00 by lzito             #+#    #+#             */
/*   Updated: 2024/04/24 15:56:11 by robin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Centralinclude.hpp"

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
		std::ifstream file(ERR_400);
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::string response = "HTTP/1.1 400 OK\r\nContent-Type: text/html\r\n\r\n" + content;
		file.close();
		std::cout << BLUE << "Response 400 sent." << RESET << std::endl;
		return response;
    }
	else
	{
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + content;
		file.close();
		std::cout << BLUE << "Response 200 sent." << RESET << std::endl;
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

std::string execute_cgi_script(const std::string& cgi_script_path, RequestParser& Req)
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("Error in pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Error in fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        // Child process
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Set environment variables
        setenv("REQUEST_METHOD", Req.getMethod().c_str(), 1);
        setenv("SCRIPT_FILENAME", cgi_script_path.c_str(), 1);
        //setenv("QUERY_STRING", Req.getQueryParam().c_str(), 1);
        std::ostringstream oss;
        oss << Req.getContentLength();
        std::string contentLengthStr = oss.str();
        setenv("CONTENT_LENGTH", contentLengthStr.c_str(), 1);
        setenv("CONTENT_TYPE", Req.getContentType().c_str(), 1);
        //std::cout << "getMethod : " << Req.getMethod().c_str() << "<br>" << std::endl;
        
        // Execute the CGI script
        if(Req.getScriptName().find(".py") != std::string::npos){
            std::string num1;
            std::string num2;
            std::string oprtr;
            std::map<std::string, std::string> query = Req.getQueryParam();
            std::map<std::string, std::string>::iterator it = query.begin();
            for (it = query.begin(); it != query.end(); it++)
            {
                if (it->first == "number1")
                    num1 = it->second;
                else if (it->first == "number2")
                    num2 = it->second;
                else if (it->first == "operator")
                    oprtr = it->second;
                else
                    std::cerr << "Invalid query parameter" << std::endl;
            }
            execl("/usr/bin/python3", "python3", cgi_script_path.c_str(), num1.c_str(), num2.c_str(), oprtr.c_str(), NULL);
        }
        else if(Req.getScriptName().find(".php") != std::string::npos)
            execl("/usr/bin/php", "php", cgi_script_path.c_str(), NULL);
        perror("Error in execl");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process
        close(pipefd[1]);

        // Read the output of the CGI script
        char buffer[4096];
        ssize_t bytes_read;
        std::string cgi_output;
        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0)
        {
            cgi_output.append(buffer, bytes_read);
        }

        close(pipefd[0]);
        waitpid(pid, NULL, 0);
        return cgi_output;
    }
}

int init_ws(ConfigFile& conf)
{
    int kq = kqueue();
	const int enable = 1;
    struct kevent events[MAX_EVENTS];
    struct kevent change_event;
    if (kq == -1)
    {
        perror("Error in kqueue");
        exit(EXIT_FAILURE);
    }

	std::vector<int> server_fd; 
	for(std::vector<int>::iterator it = conf.prt_vec.begin(); it != conf.prt_vec.end(); it++)
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
                std::string buffer = readHttpRequest(client_socket);

				try
				{
					RequestParser Req(buffer);

					t_server srvr_used = choose_server(conf, Req.getHost());
					// std::cout << request << std::endl;
					// std::cout << std::endl;
					Req.show();

					if (Req.getMethod() == "POST" && Req.getScriptName() == "upload") 
					{
						handleFileUpload(Req);
						showUploadedFiles(client_socket);
						close(client_socket);
						std::cout << BLUE << "Response upload sent." << RESET << std::endl;
					}
					else if(Req.getMethod() == "GET" && Req.getURI().find("/upload/") != std::string::npos && !Req.isCGI()){
						std::string filename = Req.getURI().substr(8);
						handleFileDownload(Req, client_socket, filename);
					}

					// Vérifier si le chemin de l'URI correspond à un script CGI
					else if (Req.getVersion().compare(HTTP_VER) != 0)
					{
						// ERREUR 500
						std::string response = readHtmlFile(Req.getURI().substr(1), srvr_used, true);
						send(client_socket, response.c_str(), response.size(), 0);
						close(client_socket);
						std::cout << BLUE << "Response 500 sent." << RESET << std::endl;
					}
					else if (Req.isCGI())
					{
						// Exécuter le script CGI
						std::string cgi_script_path = "cgi_bin/hello.py";
						//std::string cgi_output = "<h1>CGI handling</h1>";//execute_cgi_script(cgi_script_path);
                        std::string cgi_output = execute_cgi_script(cgi_script_path, Req);
						// Envoyer la sortie du script CGI au client
						std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + cgi_output;
						if (send(client_socket, response.c_str(), response.size(), 0) == -1)
						{
							perror("Error in send");
							close(client_socket);
							continue;
						}
						//std::cout << BLUE <<  << RESET << std::endl;
						close(client_socket);
					}
					else
					{
						// Réponse normale (non-CGI)
						std::string response = readHtmlFile(Req.getURI().substr(1), srvr_used, false);

						if (send(client_socket, response.c_str(), response.size(), 0) == -1)
						{
							perror("Error in send");
							close(client_socket);
							return 1;
						}

						close(client_socket);
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
					// char *begin = buffer;
					// char *end = begin + sizeof(buffer);
					// std::fill(begin, end, 0);
				}
            }
        }
    }
	for(std::vector<int>::iterator it = conf.prt_vec.begin(); it != conf.prt_vec.end(); it++)
		close(*it);
    close(kq);
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

