/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:03:13 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/01 21:50:21 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"
#include <iostream>

int testconffile(int, char**, std::string*);

int	main(int ac, char** av)
{
	std::string conf_file;
	testconffile(ac, av, &conf_file);
	ConfigFile conf(conf_file);
	//init_ws(conf);
	//std::cout << conf.getMap("srvr") << std::endl;
	return 0;
}

int testconffile(int ac, char** av, std::string* conf_file)
{
	try
	{
		if (ac > 2)
			throw ParsingException(0);
		else if (ac == 2)
			*conf_file = av[1];
		else
			*conf_file = "Conf/default.conf";
	}
	catch (std::exception& e)
	{
		return (std::cout << e.what() << std::endl && 1);
	}
	return 0;
}
