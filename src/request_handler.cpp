/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_handler.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 10:13:12 by lzito             #+#    #+#             */
/*   Updated: 2024/05/02 16:16:19 by robin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Centralinclude.hpp"

void	requestHandler(int client_socket, const ConfigFile &conf)
{
		RequestParser Req(client_socket);

		t_server srvr_used = choose_server(conf, Req.getHost());
		if (Req.getVersion().compare(HTTP_VER) != 0)
			throw (505);
		if (srvr_used.method.compare("ALL") != 0 && srvr_used.method.find("." + Req.getMethod() + " ") == std::string::npos)
			throw (405);						

		//Req.show();
		
		// if (Req.getMethod() == "POST" && Req.getScriptName() == "upload") 
		// {
		// 	handleFileUpload(Req);
		// 	showUploadedFiles(client_socket);
		// 	close(client_socket);
		// 	std::cout << BLUE << "Response upload sent." << RESET << std::endl;
		// }
		// else if(Req.getMethod() == "GET" && Req.getURI().find("/upload/") != std::string::npos && !Req.isCGI()){
		// 	std::string filename = Req.getURI().substr(8);
		// 	handleFileDownload(Req, client_socket, filename);
		// }
		// else if(Req.getMethod() == "POST" && Req.getURI().find("/delete") != std::string::npos && !Req.isCGI()){
		// 	std::string body = Req.getBody();
		// 	std::string filename = body.find("file_to_delete=") != std::string::npos ? body.substr(15) : "";
		// 	handleFileDelete(filename, client_socket);
		// }

		// Vérifier si le chemin de l'URI correspond à un script CGI
		if (Req.isCGI())
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
			// Réponse normale (non-CGI)
			std::string response = readHtmlFile(Req.getURI().substr(1).c_str(), srvr_used);

			if (send(client_socket, response.c_str(), response.size(), 0) == -1)
				throw (501);
			close(client_socket);
		}
}
