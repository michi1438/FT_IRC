/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingException.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 11:48:07 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/01 11:34:56 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ParsingException_hpp__
# define __ParsingException_hpp__

# include <iostream>
# include <cstdlib>

class ParsingException : public std::exception	
{
	private:
		int ern;
		int err_line;
		ParsingException();

	public:
		ParsingException(int _ern);				
		ParsingException(int _ern, int err_line);				
		const char *what() const throw ();
};

#endif
