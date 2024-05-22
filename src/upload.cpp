/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   upload.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 11:18:54 by robin             #+#    #+#             */
/*   Updated: 2024/05/22 16:18:09 by robin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Centralinclude.hpp"

void handleFileUpload(RequestParser & Req) {
    // Extraire le nom de fichier du Content-Disposition
    std::string filename;
    size_t filename_start = Req.getBody().find("filename=\"", 0);
    if (filename_start != std::string::npos) {
        filename_start += strlen("filename=\""); // Avancer jusqu'au début du nom de fichier
        size_t filename_end = Req.getBody().find("\"", filename_start);
        if (filename_end != std::string::npos) {
            filename = Req.getBody().substr(filename_start, filename_end - filename_start);
        }
    } else {
        std::cerr << "Filename not found in request" << std::endl;
        throw 400;
    }
    
    // Trouver la position de départ des données du fichier
	size_t header_end = Req.getBody().find("\r\n\r\n");
	if (header_end == std::string::npos) {
        std::cerr << "Invalid file data format0" << std::endl;
        throw 400;
	}
	header_end += 4;

    
    // Trouver la fin des données du fichier
    size_t boundary_end = Req.getBody().find(Req.getBoundary(), header_end);
    if (boundary_end == std::string::npos) {
        std::cerr << "Invalid file data format4" << std::endl;
        throw 400;
    }
    boundary_end -= 4;


    // Ouvrir le fichier de sortie
    std::ofstream outfile(("upload/" + filename).c_str(), std::ios::binary);
    if (!outfile.is_open()) {
        std::cerr << "Unable to save file: " << filename << std::endl;
        throw 500;
    }

    // Écrire les données du fichier dans un nouveau fichier avec le nom dynamique
    size_t pos = header_end;
    while (pos < boundary_end) {
        size_t chunk_size = std::min(static_cast<size_t>(boundary_end - pos), static_cast<size_t>(1024 * 1024)); // Taille du chunk (1 Mo)
        outfile.write(Req.getBody().data() + pos, chunk_size); // Écrire le chunk dans le fichier
        pos += chunk_size; // Avancer la position
    }

    // Fermer le fichier de sortie
    outfile.close();

    // Vérifier la taille du fichier
    std::ifstream infile(("upload/" + filename).c_str(), std::ios::binary | std::ios::ate);
    std::streamsize size = infile.tellg();
    infile.close();

    if (size == static_cast<std::streamsize>(boundary_end - header_end)) {
        std::cout << "File saved successfully: " << filename << std::endl;
    } else {
        std::cerr << "File size mismatch: " << filename << std::endl;
        throw 500;
    }
}

std::string intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void handleFileDownload(RequestParser & Req, int client_socket, std::string filename) {
    (void) Req;
    std::cout << filename << std::endl;
        // Ouvrir le fichier
        std::ifstream infile(("upload/" + filename).c_str(), std::ios::binary);
        if (!infile.is_open()) {
            //showUploadedFiles(client_socket);
            std::cerr << "Unable to open file: " << filename << std::endl;
            throw 404;
        }
        // Lire le contenu du fichier
        std::string file_content((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());

        // Fermer le fichier
        infile.close();

    std::string http_response = "HTTP/1.1 200 OK\r\n";
    http_response += "Content-Type: text/html\r\n";
    http_response += "Content-Length: " + intToString(file_content.size()) + "\r\n";
    http_response += "Content-Disposition: attachment; filename=\"" + filename + "\" \r\n";
    http_response += "\r\n";
    http_response += file_content;
    http_response += "\r\n";
    
	int bytes_sent = send(client_socket, http_response.c_str(), http_response.size(), 0);
	if (bytes_sent == 0)
	{
		std::cout << "Zero bytes were sent, this ain't normal" << std::endl; // TODO find better message...
		throw (500);
	}
	if (bytes_sent == -1)
		throw (501);
}

void showUploadedFiles(int client_socket) {
    // Récupérer la liste des fichiers dans le dossier "upload"
    std::vector<std::string> files;
    std::string folder_path = "upload/";
    DIR* dir;

    struct dirent* ent;

    if ((dir = opendir(folder_path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                files.push_back(ent->d_name);
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Unable to open directory: " << folder_path << std::endl;
        throw 500;
        return;
    }

    // Afficher les fichiers
    std::string response;
    std::string http_response;
    
    response += "<style>\r\n";
    response += "body { background-color: lavender; }\r\n"; // Définit la couleur d'arrière-plan
    response += "</style>\r\n";
    response += "<h1>Uploaded Files :</h1>\r\n"; // Ajoutez le titre
    for (std::vector<std::string>::const_reverse_iterator it = files.rbegin(); it != files.rend(); ++it) {
        response += *it + "<br>";
        response += "<a href=\"/upload/" + *it + "\" download=\"" + *it + "\" target=\"_blank\">" + "<button>Download</button>" + "</a> ";

        // Ajouter le bouton de suppression avec un formulaire
        response += "<form method=\"post\" action=\"/delete\" style=\"display: inline;\">"; // Le formulaire sera envoyé à "/delete"
        response += "<input type=\"hidden\" name=\"file_to_delete\" value=\"" + *it + "\">"; // Ajouter le nom du fichier à supprimer
        response += "<input type=\"submit\" value=\"Delete\">"; // Bouton de suppression
        response += "</form>";

        response += "<br>\r\n"; // Ajoute un saut de ligne après chaque lien de fichier

    }
    http_response = "HTTP/1.1 200 OK\r\n";
    http_response += "Content-Length: " + intToString(response.size()) + "\r\n";
    http_response += "Content-Type: text/html\r\n";
    http_response += "\r\n";
    http_response += response;
    http_response += "\r\n";
    
	int bytes_sent = send(client_socket, http_response.c_str(), http_response.size(), 0);
	if (bytes_sent == 0)
	{
		std::cout << "Zero bytes were sent, this ain't normal" << std::endl; // TODO find better message...
		throw (500);
	}
	if (bytes_sent == -1)
		throw (501);
}

void handleFileDelete(std::string filename, int client_socket) {
    std::string upload_dir = "upload/";

    // Chemin complet du fichier à supprimer
    std::string file_path = upload_dir + filename;
    // Vérifie si le fichier existe
    std::ifstream infile(file_path.c_str(), std::ios::binary);
    if (infile.is_open()) {
        infile.close();
        // Supprime le fichier
        if (std::remove(file_path.c_str()) == 0) {
            std::cout << "File deleted successfully: " << filename << std::endl;
            showUploadedFiles(client_socket);
        } else {
            std::cerr << "Error deleting file: " << filename << std::endl;
            throw 500;
        }
    } else {
        std::cerr << "File not found: " << filename << std::endl;
        throw 404;
    }
}
