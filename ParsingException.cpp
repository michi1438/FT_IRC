/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingException.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 11:56:33 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/01 11:35:08 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParsingException.hpp"

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
		default:
			return ("ERR: Unspecified parsing error");
	}
}
