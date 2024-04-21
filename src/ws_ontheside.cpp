/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_ontheside.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/21 16:42:10 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/21 16:44:57 by mguerga          ###   ########.fr       */
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
		std::cout << BLUE << "Response 500 sent." << RESET << std::endl;
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
	t_server ret;
	ret.err = true;
	return ret;
}
