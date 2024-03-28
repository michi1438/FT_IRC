/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:49:23 by mguerga           #+#    #+#             */
/*   Updated: 2024/03/28 19:35:30 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"

ConfigFile::ConfigFile(const std::string _file_name) : file_name(_file_name) 
{
	exit_status = 0;
	if (file_name.size() < 6 || file_name.substr(file_name.size() - 5).compare(".conf") != 0)
		throw ParsingException(1);
	std::ifstream conf_file(file_name.c_str());
	
	if (!conf_file.is_open())
		throw ParsingException(2);
	std::string line;
	while (std::getline(conf_file, line))
	{
		std::cout << line << std::endl;
	}
	conf_file.close();
}

ConfigFile::~ConfigFile(void)
{}

int ConfigFile::getExit_status(void) const
{
	return exit_status;
}

