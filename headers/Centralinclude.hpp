/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Centralinclude.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 11:14:39 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/24 16:07:12 by robin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <iomanip>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <err.h>
#include <vector>
#include <string>
#include <sys/event.h>
#include <iterator>
#include <dirent.h>


#include "ConfigFile.hpp"
#include "RequestParser.hpp"

#define RESET   "\033[0m"
#define RED     "\033[31m"      // Rouge
#define GREEN   "\033[32m"      // Vert
#define YELLOW  "\033[33m"      // Jaune
#define BLUE    "\033[34m"      // Bleu
#define MAGENTA "\033[35m"      // Magenta
#define CYAN    "\033[36m"      // Cyan

#define MAX_EVENTS 64
#define ERR_400 "html/40x.html"
#define ERR_500 "html/50x.html"
#define HTTP_VER "HTTP/1.1"

//webserv.cpp
int				init_ws(ConfigFile& conf);
std::string		readHtmlFile(std::string filename, t_server srvr_used, bool err_50x);
t_server		choose_server(const ConfigFile& conf, std::string host);
int				prts_is_open(std::vector<int> server_fd, int fd);

//upload.cpp
std::string		readHttpRequest(int client_socket);
void			handleFileUpload(RequestParser & Req);
void            handleFileDownload(RequestParser & Req, int client_socket, std::string filename);
void            showUploadedFiles(int client_socket);

//cgi_handler.cpp
std::string		execute_cgi_script(const std::string& cgi_script_path, RequestParser& Req);