/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:49:23 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/01 16:36:28 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"

ConfigFile::ConfigFile(const std::string _file_name) : file_name(_file_name) 
{
	int i = 0;
	if (file_name.size() < 6 || file_name.substr(file_name.size() - 5).compare(".conf") != 0)
		throw ParsingException(1);
	std::ifstream conf_file(file_name.c_str());
	
	if (!conf_file.is_open())
		throw ParsingException(2);
	std::string line;
	while (std::getline(conf_file, line) && i++ < CONFIG_FILE_MAX_SIZE)
	{
		std::string sub = line.substr(0, line.find(" "));	
		
		_map[sub.substr(0, 4)] = line.substr(line.rfind(" ") + 1, line.size());
		//std::cout << _map[sub.substr(0, 4)] << std::endl;
	}
	this->checker();
	if (i == CONFIG_FILE_MAX_SIZE)
	   throw ParsingException(4);	
	conf_file.close();
}

ConfigFile::~ConfigFile(void)
{}

void ConfigFile::checker(void)
{
	int must_have_size = 4;
	int i = -1;
	std::map<std::string, std::string>::iterator it = _map.begin();
	std::string must_have[must_have_size] = {"srvr", "prtn", "root", "lcbs"}; // TODO add all the must_have elements...

	while (it != _map.end() && i++ < must_have_size - 1)
	{
		if (_map.find(must_have[i]) == _map.end())
		{
			std::cout << "ERR: Could'nt find" << must_have[i] << "in the config file";
			throw ParsingException(5);
		}
	}
}
