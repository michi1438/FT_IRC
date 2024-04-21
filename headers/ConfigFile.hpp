/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 18:16:04 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/21 13:01:10 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ConfigFile_hpp__
# define __ConfigFile_hpp__

# define CONFIG_FILE_MAX_SIZE 100

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <map>
# include <vector>
//# include <algorithm>
# include "ParsingException.hpp"

typedef struct S_erver
{
	bool is_default;
	bool err;
	std::string root;
	std::vector<std::string> srvr_name;
	std::vector<int> prtn;
	std::string home;
	int lcbs;
} t_server;

class ConfigFile
{
	private:

		std::string file_name;
		std::vector<t_server> blocks;
		std::vector<int> prt_vec;

		ConfigFile(const ConfigFile& o);
		ConfigFile& operator=(const ConfigFile& o);

	public:

		ConfigFile(const std::string);
		~ConfigFile();
		
		bool cont_prt(int);
		bool cont_prt_of_srvr(std::vector<int>, int);
		bool cont_name_of_srvr(std::vector<std::string>, std::string);
		std::string prt_vec_print();

		std::vector<t_server> getBlocks(void) const;
		std::vector<int> getPort_vec(void) const;
		//void checker() const;
		//const char* getMap(std::string);		

};

#endif
