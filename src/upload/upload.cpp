/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   upload.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 11:18:54 by robin             #+#    #+#             */
/*   Updated: 2024/04/21 16:33:42 by lzito            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/Centralinclude.hpp"

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
        return;
    }

    // Trouver la position de départ des données du fichier
	size_t header_end = Req.getBody().find("\r\n\r\n");
	if (header_end == std::string::npos) {
    std::cerr << "Invalid file data format0" << std::endl;
    return;
	}
	header_end += 4;

    
    // Trouver la fin des données du fichier
    size_t boundary_end = Req.getBody().find(Req.getBoundary(), header_end);
    if (boundary_end == std::string::npos) {
        std::cerr << "Invalid file data format4" << std::endl;
        return;
    }
    boundary_end -= 4;


    // Ouvrir le fichier de sortie
    std::ofstream outfile(("upload/" + filename).c_str(), std::ios::binary);
    if (!outfile.is_open()) {
        std::cerr << "Unable to save file: " << filename << std::endl;
        return;
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
    }
}
