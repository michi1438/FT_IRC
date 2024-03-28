/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingException.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 11:48:07 by mguerga           #+#    #+#             */
/*   Updated: 2024/03/28 12:05:44 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ParsingException_hpp__
# define __ParsingException_hpp__

# include <iostream>

class ParsingException : public std::exception	
{
	private:
		int ern;
		ParsingException();

	public:
		ParsingException(int _ern);				
		const char *what() const throw ();
};

#endif
