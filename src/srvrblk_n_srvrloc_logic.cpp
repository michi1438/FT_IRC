/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   srvrblk_n_srvrloc_logic.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/30 11:36:12 by mguerga           #+#    #+#             */
/*   Updated: 2024/05/03 14:47:05 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Centralinclude.hpp"
#include <sys/stat.h> // for the S_ISDIR() macro...

bool is_location(std::string filename, t_server srvr)
{
	for(std::vector<t_loc>::const_iterator loc_it = srvr.locations.begin(); loc_it != srvr.locations.end(); loc_it++)
	{
		if (loc_it->l_path.compare(1, std::string::npos, filename) == 0)
		{
			return true;
		}
	}
	return false;
}

std::string readHtmlFile(std::string filename, t_server srvr)
{
	std::fstream file;
	if (filename.empty() == true)
	{
		filename.append(srvr.root).append(srvr.home);
		file.open(filename.c_str());
	}
	else if (is_location(filename, srvr) == true)
	{
		filename.append("/").append(srvr.home);
		file.open(filename.insert(0, srvr.root).c_str());
	}
	else
		file.open(filename.insert(0, srvr.root).c_str());

	struct stat buf;
	stat(filename.c_str(), &buf);
	// TODO Check directory listing // if (S_ISDIR(buf.st_mode) != 0 && srvr.repo_listing)
	if (S_ISDIR(buf.st_mode) != 0 && srvr.list_repo == true)
	{
		int pid;
		int wstatus;
		if ((pid = fork()) == 0)
		{
			if (execl("cgi_bin/dir_listing.cgi", "dir_listing.cgi", filename.c_str(), NULL) == -1) 	
				perror("execl");
		}
		WIFEXITED(wstatus);
		waitpid(pid, &wstatus, 0);
		file.open("cgi_bin/dir_listing.html");
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + content;
		file.close();
		std::cout << BLUE << "Response 200 sent." << RESET << std::endl;
		return response;
	}
	else if (S_ISDIR(buf.st_mode) != 0)
		throw (403);
	else if (!file.is_open())
		throw (404);
	else
	{
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + content;
		file.close();
		std::cout << BLUE << "Response 200 sent." << RESET << std::endl;
		return response;
	}
	//return "crap";
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

t_server	 choose_server(const ConfigFile& conf, const std::string req_host)
{
	std::string host_name = req_host.substr(0, req_host.find(':'));
	int	host_port = atoi(req_host.substr(req_host.find(':') + 1).c_str());
	std::vector<t_server> servers = conf.getBlocks();
	for(std::vector<t_server>::const_iterator srvr_it = servers.begin(); srvr_it != servers.end(); srvr_it++)
	{
		for(std::vector<t_prt>::const_iterator prtn_it = srvr_it->prt_n_default.begin(); prtn_it != srvr_it->prt_n_default.end(); prtn_it++)
		{
			if ((*prtn_it).prtn == host_port)
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
		for(std::vector<t_prt>::const_iterator prtn_it = srvr_it->prt_n_default.begin(); prtn_it != srvr_it->prt_n_default.end(); prtn_it++)
		{
			if ((*prtn_it).prtn == host_port && (*prtn_it).is_deflt == true)
			{
				if (!srvr_it->srvr_name.empty())
					std::cout << host_name << ":" << host_port << " was passed to the default-tagged server-block (" << srvr_it->srvr_name[0] << ") with its port number." << std::endl;
				else
					std::cout << host_name << ":" << host_port << " went to the default block of this prtn with no srvr_name defined" << std::endl;
				return *srvr_it;
			}
		}
	}
	throw (500);
}
