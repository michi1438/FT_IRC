/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:49:23 by mguerga           #+#    #+#             */
/*   Updated: 2024/05/01 13:30:27 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/ConfigFile.hpp"
#include <climits>

void ConfigFile::print_blocks(t_server *serverinfo)
{
	std::cout << "SERVER_BLOCK #" << std::endl;
	std::cout << "\tSrvr_name:";
	for(std::vector<std::string>::const_iterator name_it = serverinfo->srvr_name.begin(); name_it != serverinfo->srvr_name.end(); name_it++)
		std::cout << " " << *name_it;
	std::cout << std::endl;
	std::cout << "\tPrtn:";
	for(std::vector<t_prt>::const_iterator port_it = serverinfo->prt_n_default.begin(); port_it!= serverinfo->prt_n_default.end(); port_it++)
	{
		std::cout << " " << port_it->prtn; 
		if (port_it->is_deflt == true)
			std::cout << "(d)"; 
	}
	std::cout << std::endl;
	std::cout << "\t" << "Root: " << serverinfo->root << std::endl;
	std::cout << "\t" << "Home/Index: " << serverinfo->home << std::endl;
	std::cout << "\t" << "Accepted methods: " << serverinfo->method << std::endl;
	std::cout << "\t" << "Accepted cgi_extension: " << serverinfo->cgi_wl << std::endl;
	std::cout << "\t" << "Up/Down/Del directory: " << serverinfo->load_dir << std::endl;
	std::cout << "\t" << "(lcbs)limit_client_body_size: " << serverinfo->lcbs << std::endl;
	if (serverinfo->locations.empty() == false)
		this->print_loc(serverinfo->locations);
	std::cout << std::endl;
	std::cout << std::endl;
}

void ConfigFile::print_loc(std::vector<t_loc> loc)
{
	for(std::vector<t_loc>::const_iterator loc_it = loc.begin(); loc_it != loc.end(); loc_it++)
	{
		std::cout << "\t" << "# LOCATION_BLOCK " << (*loc_it).l_path << std::endl;
		std::cout << "\t\t" << "l_root(unused/ununderstood): " << (*loc_it).l_root << std::endl;
		std::cout << "\t\t" << "l_home: " << (*loc_it).l_home << std::endl;
		std::cout << "\t\t" << "l_method: " << (*loc_it).l_method << std::endl;
		std::cout << "\t\t" << "l_cgi_wl: " << (*loc_it).l_cgi_wl << std::endl;
		std::cout << "\t\t" << "l_lcbs: " << (*loc_it).l_lcbs << std::endl;
	}
}

void ConfigFile::finalize_blocks(t_server *serverinfo)
{
	serverinfo->err = false;	
	if (serverinfo->method.empty() == true)
		serverinfo->method = "ALL";
	if (serverinfo->cgi_wl.empty() == true)
		serverinfo->cgi_wl = "ALL";
	if (serverinfo->load_dir.empty() == true)
		serverinfo->load_dir = "upload/";
	this->blocks.push_back(*serverinfo);
	this->print_blocks(serverinfo);
	serverinfo->locations.clear();
	serverinfo->srvr_name.clear();
	serverinfo->prt_n_default.clear();
	serverinfo->method.clear();
	serverinfo->cgi_wl.clear();
	serverinfo->load_dir.clear();
}

ConfigFile::ConfigFile(const std::string _file_name) : file_name(_file_name) 
{
	int i = 0;
	if (file_name.size() < 6 || file_name.substr(file_name.size() - 5).compare(".conf") != 0)
		throw ParsingException(1);
	std::ifstream conf_file(file_name.c_str());
	
	if (!conf_file.is_open() || !conf_file.good())
		throw ParsingException(2);
	std::string line;
	t_server serverinfo;
	serverinfo.lcbs = INT_MAX;
	while (std::getline(conf_file, line) && i++ < CONFIG_FILE_MAX_SIZE)
	{
		std::istringstream iss(line);
		std::string sub;
		if (line.find("}") == 0)
			this->finalize_blocks(&serverinfo);
		else if (line.find('\t') == 0)
		{
			iss >> sub;
			if(sub.find("srvr_name:") == 0)
			{
				while(iss >> sub)
				{
					if (!cont_name_of_srvr(serverinfo.srvr_name, sub))
						serverinfo.srvr_name.push_back(sub);
				}
			}
			else if (sub.find("prtn_") == 0)
			{
				while(iss >> sub)
				{
					if (sub.find_first_not_of(" 0123456789") != std::string::npos)
						throw ParsingException(8);
					t_prt this_prt;
					int sub_int = atoi(sub.c_str());
					this_prt.is_deflt = false;
					if (!cont_prt(sub_int))
					{
						this_prt.is_deflt = true;
						prt_vec.push_back(sub_int);
					}
					if (!cont_prt_of_srvr(serverinfo.prt_n_default, sub_int))
					{
						this_prt.prtn = sub_int;
						serverinfo.prt_n_default.push_back(this_prt);
					}
				}
			}
			else if (sub.find("loca_") == 0)
			{
				t_loc this_loc;
				this_loc.l_lcbs = INT_MAX;
				iss >> sub;
				this_loc.l_path = sub;
				for (std::vector<t_loc>::iterator loc_it = serverinfo.locations.begin(); loc_it != serverinfo.locations.end(); loc_it++)
					if (loc_it->l_path.compare(sub) == 0)
						throw ParsingException(14);
				while (std::getline(conf_file, line) && i++ < CONFIG_FILE_MAX_SIZE)
				{
					std::istringstream iss(line);
					if (line.find("\t}") == 0)
					{
						if (this_loc.l_method.empty() == true)
							this_loc.l_method = "ALL";
						if (this_loc.l_cgi_wl.empty() == true)
							this_loc.l_cgi_wl = "ALL";
						serverinfo.locations.push_back(this_loc);
						break;
					}
					else if (line.find("\t\t") == 0)
					{
						iss >> sub;
						if (sub.find("root_") == 0)
						{
							iss >> sub;
							this_loc.l_root = sub;
						}
						else if (sub.find("cgi_") == 0)
						{
							while(iss >> sub)
								this_loc.l_cgi_wl.append("." + sub + " ");
						}
						if (sub.find("home_") == 0)
						{
							iss >> sub;
							this_loc.l_home = sub;
						}
						if (sub.find("lcbs_") == 0)
						{
							iss >> sub;
							if (sub.find_first_not_of(" 0123456789") != std::string::npos)
								throw ParsingException(9);
							this_loc.l_lcbs= atoi(sub.c_str());
						}
						if (sub.find("meth_") == 0)
						{
							while(iss >> sub)
							{
								this_loc.l_method.append("." + sub + " ");
							}
						}
					}
					else
						throw ParsingException(12);
				}
			}	
			else if (sub.find("meth_") == 0)
			{
				while(iss >> sub)
					serverinfo.method.append("." + sub + " ");
			}
			else if (sub.find("load_") == 0) // TODO implement in the rest of upload...
			{
				iss >> sub;
				serverinfo.load_dir = sub;
			}
			else if (sub.find("root_") == 0)
			{
				iss >> sub;
				serverinfo.root = sub;
			}
			else if (sub.find("home_") == 0)
			{
				iss >> sub;
				serverinfo.home = sub;
			}
			else if (sub.find("cgi_") == 0)
			{
				while(iss >> sub)
					serverinfo.cgi_wl.append("." + sub + " ");
			}
			else if (sub.find("lcbs_") == 0)
			{
				iss >> sub;
				if (sub.find_first_not_of(" 0123456789") != std::string::npos)
					throw ParsingException(9);
				serverinfo.lcbs= atoi(sub.c_str());
			}
			else
				throw ParsingException(13);
		}
	}
//	this->checker();
	if (i >= CONFIG_FILE_MAX_SIZE)
	   throw ParsingException(4);	
	if (this->blocks.empty() == true)
		throw ParsingException(10);
	if (this->prt_vec.empty() == true)
		throw ParsingException(11);
	conf_file.close();
}

ConfigFile::~ConfigFile(void)
{}

std::string ConfigFile::prt_vec_print()
{
	for(std::vector<int>::iterator it = prt_vec.begin(); it != prt_vec.end(); it++)
	{
		std::cout << *it;
		if (it + 1 != prt_vec.end())
			std::cout << " ";
	}
	return ".";
}

bool ConfigFile::cont_prt_of_srvr(std::vector<t_prt> vec, int cmp)
{
	for(std::vector<t_prt>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		if ((*it).prtn == cmp)
			return true;
	}
	return false;
}

bool ConfigFile::cont_name_of_srvr(std::vector<std::string> vec, std::string cmp)
{
	for(std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		if (*it == cmp)
			return true;
	}
	return false;
}

bool ConfigFile::cont_prt(int cmp)
{
	for(std::vector<int>::iterator it = prt_vec.begin(); it != prt_vec.end(); it++)
	{
		if (*it == cmp)
			return true;
	}
	return false;
}
	
std::vector<t_server> ConfigFile::getBlocks(void) const
{
	return blocks;
}

std::vector<int> ConfigFile::getPort_vec(void) const
{
	return prt_vec;
}
