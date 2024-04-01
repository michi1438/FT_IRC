/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:03:13 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/01 12:45:36 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"
#include <iostream>

int	main(int ac, char** av)
{
	try
	{
		if (ac != 2)
			throw ParsingException(0);
		ConfigFile conf(av[1]);
	}

	catch (std::exception& e)
	{
		return (std::cout << e.what() << std::endl && 1);
	}
	return 0;
}

