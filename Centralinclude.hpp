/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Centralinclude.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 11:14:39 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/03 13:33:09 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/event.h>
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

#include "ConfigFile.hpp"

#define RESET   "\033[0m"
#define RED     "\033[31m"      // Rouge
#define GREEN   "\033[32m"      // Vert
#define YELLOW  "\033[33m"      // Jaune
#define BLUE    "\033[34m"      // Bleu
#define MAGENTA "\033[35m"      // Magenta
#define CYAN    "\033[36m"      // Cyan

#define MAX_EVENTS 64
#define PORT 8080

//webserv.cpp
int			init_ws(ConfigFile& conf);
std::string	readHtmlFile(const char *filename);
