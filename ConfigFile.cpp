/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:49:23 by mguerga           #+#    #+#             */
/*   Updated: 2024/03/28 11:59:32 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"

ConfigFile::ConfigFile(const std::string _file_name) : file_name(_file_name) 
{
	exit_status = 0;
	if (file_name.find(".conf") == std::string::npos || file_name.size() < 6)
	{
		throw ParsingException(1);
	}
}

ConfigFile::~ConfigFile(void)
{}

int ConfigFile::getExit_status(void) const
{
	return exit_status;
}

