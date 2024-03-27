/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:03:13 by mguerga           #+#    #+#             */
/*   Updated: 2024/03/27 18:31:24 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"
#include <iostream>

int	main(int ac, char** av)
{
	if (ac != 2)
	{
		std::cout << "ERR: 1 config_file.conf is expected" << std::endl;
		return (1);
	}
	ConfigFile conf(av[1]);
	return conf.getExit_status();
}

