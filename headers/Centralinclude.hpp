/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Centralinclude.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 11:14:39 by mguerga           #+#    #+#             */
/*   Updated: 2024/05/22 12:05:45 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <iomanip>
#include <sys/socket.h>
#include <sys/wait.h>
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
#include <signal.h>

#include "ConfigFile.hpp"
#include "RequestParser.hpp"

#define KEY		0
#define VALUE	1
#define RESET   "\033[0m"
#define RED     "\033[31m"      // Rouge
#define GREEN   "\033[32m"      // Vert
#define YELLOW  "\033[33m"      // Jaune
#define BLUE    "\033[34m"      // Bleu
#define MAGENTA "\033[35m"      // Magenta
#define CYAN    "\033[36m"      // Cyan

#define MAX_EVENTS 64
#define ERR_DIR "default_errpages/"
#define ERR_301 "301.html"
#define ERR_400 "40x.html"
#define ERR_403 "403.html"
#define ERR_404 "404.html"
#define ERR_405 "405.html"
#define ERR_408 "408.html"
#define ERR_413 "413.html"
#define ERR_414 "414.html"
#define ERR_501 "501.html"
#define ERR_504 "504.html"
#define ERR_505 "505.html"
#define ERR_500 "50x.html"
#define ERR_512 "512.html"

#define HTTP_VER "HTTP/1.1"

//webserv.cpp
int				init_ws(ConfigFile&);

//upload.cpp
void			handleFileUpload(RequestParser&);
void            handleFileDownload(RequestParser&, int, std::string);
void            handleFileDelete(std::string, int);
void            showUploadedFiles(int);

//srvrblk_n_srvrloc_logic.cpp
bool			is_location(std::string, t_server);
std::string		readHtmlFile(std::string, t_server);
int				prts_is_open(std::vector<int>, int);
t_server		choose_server(const ConfigFile&, std::string);

//Err_page_switch.cpp
std::string		read_errpage(int, RequestParser&, t_server);

//cgi_handler.cpp
std::string		execute_cgi_script(const std::string&, RequestParser&);

//request_parser_utils.cpp
std::string		decodeUri(const std::string &uri);
void			readFromSocket(int, std::string);
void			addBody(int, std::string& , int);
std::string		getHttpRequest(int);

//request_handler.cpp
t_server		update_location(t_server, std::string);
void			requestHandler(int, t_server*, RequestParser&);
