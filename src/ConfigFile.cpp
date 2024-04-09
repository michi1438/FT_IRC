/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:49:23 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/09 11:06:29 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/ConfigFile.hpp"

ConfigFile::ConfigFile(const std::string _file_name) : file_name(_file_name) 
{
	int i = 0;
	if (file_name.size() < 6 || file_name.substr(file_name.size() - 5).compare(".conf") != 0)
		throw ParsingException(1);
	std::ifstream conf_file(file_name.c_str());
	
	if (!conf_file.is_open() || !conf_file.good())
		throw ParsingException(2);
	std::string line;
	std::string srvr_name;
	t_server serverinfo;
	while (std::getline(conf_file, line) && i++ < CONFIG_FILE_MAX_SIZE)
	{
		std::istringstream iss(line);
		std::string sub;
		if(line.find("srvr_name:") == 0)
		{
			iss >> sub;
			iss >> sub;
			srvr_name = sub;
		}
		else if (line.find("}") == 0)
		{
			this->_map[srvr_name] = serverinfo;
		}
		else if (line.find('\t') == 0)
		{
			iss >> sub;
			if (sub.find("prtn_") == 0)
			{
				iss >> sub;
				serverinfo.prtn = atoi(sub.c_str());
				if (!vec_contains(serverinfo.prtn))
					prt_vec.push_back(serverinfo.prtn);
			}
			if (sub.find("root_") == 0)
			{
				iss >> sub;
				serverinfo.root = sub;
			}
			if (sub.find("home_") == 0)
			{
				iss >> sub;
				serverinfo.home = sub;
			}
			if (sub.find("lcbs_") == 0)
			{
				iss >> sub;
				serverinfo.lcbs= atoi(sub.c_str());
			}
		}
	}
//	this->checker();
	if (i >= CONFIG_FILE_MAX_SIZE)
	   throw ParsingException(4);	
	conf_file.close();
}

ConfigFile::~ConfigFile(void)
{}

std::string ConfigFile::prt_vec_print()
{
	for(std::vector<int>::iterator it = prt_vec.begin(); it != prt_vec.end(); it++)
	{
		std::cout << *it;
		if (it + 1 != prt_vec.end())
			std::cout << " ";
	}
	return ".";
}

bool ConfigFile::vec_contains(int cmp)
{
	for(std::vector<int>::iterator it = prt_vec.begin(); it != prt_vec.end(); it++)
	{
		if (*it == cmp)
			return true;
	}
	return false;
}
	


/*void ConfigFile::checker(void) const
{
	int must_have_size = 4;
	int i = -1;
	std::map<std::string, std::string>::const_iterator it = _map.begin();
	std::string must_have[4] = {"srvr", "prtn", "root", "lcbs"}; // TODO add all the must_have elements...

	while (it != _map.end() && i++ < must_have_size - 1)
	{
		if (_map.find(must_have[i]) == _map.end())
		{
			std::cout << "ERR: Could'nt find " << must_have[i] << " in the config file";
			throw ParsingException(5);
		}
	}
}*/
