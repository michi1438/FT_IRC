/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 18:16:04 by mguerga           #+#    #+#             */
/*   Updated: 2024/03/27 18:31:12 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ConfigFile_hpp__
# define __ConfigFile_hpp__

# include <iostream>

class ConfigFile
{
	private:

		int exit_status;
		std::string file_name;

		ConfigFile(const ConfigFile& o);
		ConfigFile& operator=(const ConfigFile& o);

	public:
		ConfigFile(const std::string);
		~ConfigFile();
		int getExit_status() const;
};

#endif
