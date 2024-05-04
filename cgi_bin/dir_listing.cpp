#include <fstream>
#include <iostream>
#include <dirent.h>

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
		outfile << "<html>" << std::endl << "<head><title>Index of " << av[1] << "</title></head>" << std::endl;
		outfile << "<body>" << std::endl << "<h1>Index of " << av[1] << "</h1><hr><pre>" << std::endl;
		while ((diread = readdir(dir)) != NULL)
		{
			outfile << "<a href=\"" << diread->d_name << "\">" << diread->d_name << "</a>" << "\t\t\t\t\t" << diread->d_reclen << std::endl;
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
/*
<a href="../">../</a>
<a href="boot/">boot/</a>                                              01-Apr-2024 17:59                   -
<a href="x86_64/">x86_64/</a>                                            01-Apr-2024 17:59                   -
<a href="pkglist.x86_64.txt">pkglist.x86_64.txt</a>                                 01-Apr-2024 17:58                8084
<a href="version">version</a>                                            01-Apr-2024 17:58                  11
</pre><hr></body>
</html>
*/
