/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_handler.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzito <lzito@student.42lausanne.ch>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 10:13:12 by lzito             #+#    #+#             */
/*   Updated: 2024/05/06 11:29:24 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Centralinclude.hpp"

t_server	update_location(t_server srvr_used, std::string uri)
{
	t_server new_serv;
	new_serv = srvr_used;
	for(std::vector<t_loc>::const_iterator loc_it = new_serv.locations.begin(); loc_it != new_serv.locations.end(); loc_it++)
	{
		if (uri.find(loc_it->l_path) == 0)
		{
			new_serv.loc_root = loc_it->l_root; // TODO For now, does not overide the server's root, and is put in other attribute loc_root not put to any use...
			new_serv.home = loc_it->l_home;
			new_serv.method = loc_it->l_method;
			new_serv.cgi_wl = loc_it->l_cgi_wl;
			new_serv.lcbs = loc_it->l_lcbs;
			return new_serv;
		}
	}
	return new_serv;
}

void	requestHandler(int client_socket, const ConfigFile &conf, RequestParser &Req)
{
		Req.show();
		
		std::cout << RESET << std::endl;
		t_server srvr_used = choose_server(conf, Req.getHost());
		if (!srvr_used.locations.empty())
			srvr_used = update_location(srvr_used, Req.getURI());
		//std::cout << "cgi_wl = " << srvr_used.cgi_wl << std::endl;
		if (Req.getVersion().compare(HTTP_VER) != 0)
			throw (505);
		if (srvr_used.method.compare("ALL") != 0 && srvr_used.method.find("." + Req.getMethod() + " ") == std::string::npos)
			throw (405);						
		if (static_cast<size_t>(srvr_used.lcbs) <= Req.getContentLength())
			throw (413);
		if (Req.getURI().size() >= BUFFER_SIZE)
			throw (414);

		// TODO for the next 3 if/elseif make the directory be "srvr_used.load_dir".
		if (Req.getMethod() == "POST" && Req.getScriptName() == "upload") 
		{
			handleFileUpload(Req);
			showUploadedFiles(client_socket);
			close(client_socket);
			std::cout << BLUE << "Response upload sent." << RESET << std::endl;
		}
		else if(Req.getMethod() == "GET" && Req.getURI().find("/upload/") != std::string::npos && !Req.isCGI()){
			std::string filename = Req.getURI().substr(8);
			handleFileDownload(Req, client_socket, filename);
		}
		else if(Req.getMethod() == "POST" && Req.getURI().find("/delete") != std::string::npos && !Req.isCGI()){
			std::string body = Req.getBody();
			std::string filename = body.find("file_to_delete=") != std::string::npos ? body.substr(15) : "";
			handleFileDelete(filename, client_socket);
		}

		// TODO check for "server_used.cgi_wl" allows the right extension.
		else if (Req.isCGI())
		{
			// Exécuter le script CGI
			std::string cgi_script_path = "cgi_bin/" + Req.getScriptName();
			//std::string cgi_output = "<h1>CGI handling</h1>";//execute_cgi_script(cgi_script_path);
			std::string cgi_output = execute_cgi_script(cgi_script_path, Req);
			// Envoyer la sortie du script CGI au client
			std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + cgi_output;
			if (send(client_socket, response.c_str(), response.size(), 0) == -1)
				throw (501);
			close(client_socket);
			std::cout << BLUE << "Response sent from CGI" << RESET << std::endl;
		}
		else
		{
			std::string response = readHtmlFile(Req.getURI().substr(1).c_str(), srvr_used);

			if (send(client_socket, response.c_str(), response.size(), 0) == -1)
				throw (501);
			close(client_socket);
		}
}
