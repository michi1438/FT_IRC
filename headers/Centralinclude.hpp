/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Centralinclude.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 11:14:39 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/25 08:48:19 by lzito            ###   ########.fr       */
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
#define ERR_400 "default_errpages/40x.html"
#define ERR_403 "default_errpages/403.html"
#define ERR_404 "default_errpages/404.html"
#define ERR_405 "default_errpages/405.html"
#define ERR_505 "default_errpages/505.html"
#define ERR_500 "default_errpages/50x.html"
#define ERR_512 "default_errpages/512.html"

#define HTTP_VER "HTTP/1.1"

//webserv.cpp
int				init_ws(ConfigFile& conf);

//upload.cpp
std::string		readHttpRequest(int client_socket);
void			handleFileUpload(RequestParser & Req);
void            handleFileDownload(RequestParser & Req, int client_socket, std::string filename);
void            handleFileDelete(std::string filename, int client_socket);
void            showUploadedFiles(int client_socket);

//ws_ontheside.cpp
std::string		readHtmlFile(std::string filename, t_server srvr_used);
t_server		choose_server(const ConfigFile& conf, std::string host);
int				prts_is_open(std::vector<int> server_fd, int fd);
std::string		read_errpage(int err_code);

//cgi_handler.cpp
std::string		execute_cgi_script(const std::string& cgi_script_path, RequestParser& Req);
