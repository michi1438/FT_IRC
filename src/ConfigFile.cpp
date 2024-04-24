/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:49:23 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/24 09:46:11 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/ConfigFile.hpp"

void ConfigFile::print_blocks(t_server *serverinfo)
{
	serverinfo->err = false;	
	std::cout << "SERVER_BLOCK #" << std::endl;
	for(std::vector<std::string>::const_iterator name_it = serverinfo->srvr_name.begin(); name_it != serverinfo->srvr_name.end(); name_it++)
		std::cout << *name_it << std::endl;
	for(std::vector<t_prt>::const_iterator port_it = serverinfo->prt_n_default.begin(); port_it!= serverinfo->prt_n_default.end(); port_it++)
		std::cout << (*port_it).prtn << std::endl;
	if (serverinfo->method.empty() == true)
		serverinfo->method = "ALL";
	std::cout << "Accepted methods: " << serverinfo->method << std::endl;
	std::cout << std::endl;
	this->blocks.push_back(*serverinfo);
	serverinfo->srvr_name.clear();
	serverinfo->prt_n_default.clear();
	serverinfo->method.clear();
}

ConfigFile::ConfigFile(const std::string _file_name) : file_name(_file_name) 
{
	int i = 0;
	if (file_name.size() < 6 || file_name.substr(file_name.size() - 5).compare(".conf") != 0)
		throw ParsingException(1);
	std::ifstream conf_file(file_name.c_str());
	
	if (!conf_file.is_open() || !conf_file.good())
		throw ParsingException(2);
	std::string line;
	t_server serverinfo;
	while (std::getline(conf_file, line) && i++ < CONFIG_FILE_MAX_SIZE)
	{
		std::istringstream iss(line);
		std::string sub;
		if (line.find("}") == 0)
			this->print_blocks(&serverinfo);
		else if (line.find('\t') == 0)
		{
			iss >> sub;
			if(sub.find("srvr_name:") == 0)
			{
				while(iss >> sub)
				{
					if (!cont_name_of_srvr(serverinfo.srvr_name, sub))
						serverinfo.srvr_name.push_back(sub);
				}
			}
			if (sub.find("prtn_") == 0)
			{
				while(iss >> sub)
				{
					if (sub.find_first_not_of(" 0123456789") != std::string::npos)
						break; // TODO maybe an error
					t_prt this_prt;
					int sub_int = atoi(sub.c_str());
					this_prt.is_deflt = false;
					if (!cont_prt(sub_int))
					{
						this_prt.is_deflt = true;
						prt_vec.push_back(sub_int);
					}
					if (!cont_prt_of_srvr(serverinfo.prt_n_default, sub_int))
					{
						this_prt.prtn = sub_int;
						serverinfo.prt_n_default.push_back(this_prt);
					}
				}
			}
			if (sub.find("meth_") == 0)
			{
				while(iss >> sub)
					serverinfo.method.append("." + sub + " ");
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

bool ConfigFile::cont_prt_of_srvr(std::vector<t_prt> vec, int cmp)
{
	for(std::vector<t_prt>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		if ((*it).prtn == cmp)
			return true;
	}
	return false;
}

bool ConfigFile::cont_name_of_srvr(std::vector<std::string> vec, std::string cmp)
{
	for(std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		if (*it == cmp)
			return true;
	}
	return false;
}

bool ConfigFile::cont_prt(int cmp)
{
	for(std::vector<int>::iterator it = prt_vec.begin(); it != prt_vec.end(); it++)
	{
		if (*it == cmp)
			return true;
	}
	return false;
}
	
std::vector<t_server> ConfigFile::getBlocks(void) const
{
	return blocks;
}

std::vector<int> ConfigFile::getPort_vec(void) const
{
	return prt_vec;
}


/*void ConfigFile::checker(void) const
{
	int must_have_size = 4;
	int i = -1;
	std::map<std::string, std::string>::const_iterator it = blocks.begin();
	std::string must_have[4] = {"srvr", "prtn", "root", "lcbs"}; // TODO add all the must_have elements...

	while (it != blocks.end() && i++ < must_have_size - 1)
	{
		if (blocks.find(must_have[i]) == blocks.end())
		{
			std::cout << "ERR: Could'nt find " << must_have[i] << " in the config file";
			throw ParsingException(5);
		}
	}
}*/
