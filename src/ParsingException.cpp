/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingException.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 11:56:33 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/24 12:22:17 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/ParsingException.hpp"

ParsingException::ParsingException(int _ern) : ern(_ern)
{
	err_line = -1;
}

ParsingException::ParsingException(int _ern, int _err_line) : ern(_ern), err_line(_err_line)
{
}

const char* ParsingException::what() const throw ()
{
	switch (ern)
	{
		case 0:
			return ("ERR: Need 1 arg");
		case 1:
			return ("ERR: arg is expected in the form EXAMPLE.conf");
		case 2:
			return ("ERR: The configuration file does'nt open.");
		case 3:
		{
			std::cout << "ERR: The configuration file has a problem line " << err_line;
			return (".");
		}
		case 4:
			return ("ERR: The configuration file has too many lines.");
		case 5:
			return (".");
		case 7:
			return ("ERR: in vec_contains.");
		case 8:
			return ("ERR: One of the ports number is not of the right format in the .conf.");
		case 9:
			return ("ERR: lcbs(limit_client_body_size) is not in the right format.");
		case 10:
			return ("ERR: the given conffile is empty or doesn't fit the requierment given in the default example...");
		case 11:
			return ("ERR: No ports were given...");
		default:
			return ("ERR: Unspecified parsing error");
	}
}
