/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:49:23 by mguerga           #+#    #+#             */
/*   Updated: 2024/03/27 18:31:08 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"

ConfigFile::ConfigFile(const std::string _file_name) : file_name(_file_name) 
{
	exit_status = 0;
	if (file_name.find(".conf") == std::string::npos)
	{
		//throw ParsingException();
		exit_status = 1;
		std::cout << "One config file is expected in the form EXAMPLE.conf" << std::endl;
	}
		
}

ConfigFile::~ConfigFile(void)
{}

int ConfigFile::getExit_status(void) const
{
	return exit_status;
}
