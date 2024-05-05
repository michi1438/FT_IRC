#include <fstream>
#include <iostream>
#include <dirent.h>
#include <unistd.h>

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cout << "You have not send the right number of arguments..." << std::endl;	
		return 1;
	}
	DIR *dir;
	struct dirent *diread;
	if ((dir = opendir(av[1])) != NULL)
	{
		std::ofstream outfile("cgi_bin/dir_listing.html", std::ios::out);
		outfile << "<!doctype html><html location=\"" << "127.0.0.1:8080/" << av[1] << "\">" << std::endl; // FIXME trying to add things to the response header...
		outfile << "<head><title>Index of " << av[1] << "</title></head>" << std::endl;
		outfile << "<body>" << std::endl << "<h1>Index of " << av[1] << "</h1><hr><pre>" << std::endl;
		while ((diread = readdir(dir)) != NULL)
		{
			std::string dir_name = diread->d_name;
			if (diread->d_type == DT_DIR)
				outfile << "<a href=\"" << dir_name + "/" << "\">" << dir_name << "</a>" << diread->d_reclen << std::endl;
			else
				outfile << "<a href=\"" << dir_name << "\">" << dir_name << "</a>" << diread->d_reclen << std::endl;
		}
		outfile << "</pre><hr></body>" << "</html>" << std::endl;
		closedir(dir);
		outfile.close();
	}
	else
	{
		std::cout << "Could not open dir..." << std::endl;	
		return 1;
	}
	return 0;
}
