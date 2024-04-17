/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:03:13 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/04 15:56:02 by lzito            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Centralinclude.hpp"

int testconffile(int, char**, std::string*);

int	main(int ac, char** av)
{
	try
	{
		std::string conf_file;
		testconffile(ac, av, &conf_file);
		ConfigFile conf(conf_file);
		init_ws(conf);
	}	
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}
	return 0;
}

int testconffile(int ac, char** av, std::string* conf_file)
{
	if (ac > 2)
		throw ParsingException(0);
	else if (ac == 2)
		*conf_file = av[1];
	else
		*conf_file = "conf/default.conf";
	return 0;
}
