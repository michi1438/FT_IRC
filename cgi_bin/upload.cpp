#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h> // Include the <unistd.h> header file for getcwd()
#include <cstdlib> // Include the <cstdlib> header file for accessing environment variables

#define UPLOAD_DIR "./upload/"



int main(){
    //std::cout << getcwd(NULL, 0) << std::endl;

    // std::string finalBody;
    // std::string body = std::getenv("BODY"); // Retrieve the environment variable "BODY"
    // std::string boundary = std::getenv("BOUNDARY"); // Retrieve the environment variable "BOUNDARY
    // //std::string contentType = std::getenv("CONTENT_TYPE"); // Retrieve the environment variable "CONTENT_TYPE"
    // finalBody = body.substr(body.find(boundary) + boundary.length() + 2); // Extract the body from the request
    // finalBody = finalBody.substr(0, finalBody.find(boundary) - 2); // Extract the body from the request

    // //QUAND TU TROUVES CONTENT-TYPE: tu sautes deux lignes
    // finalBody = finalBody.substr(finalBody.find("Content-Type: ") + 14);
    // finalBody = finalBody.substr(finalBody.find("\n\n") + 2);

    // //std::cout << "\033[1;31m" << finalBody << "\n" << "\033[0m" << std::endl;

    // std::string line;
    // std::ofstream file(std::string(UPLOAD_DIR) + "upload.txt");
    // file << finalBody;
    // file.close();

// Extraire le nom de fichier du Content-Disposition
    std::string body = std::getenv("BODY");
    std::string boundary = std::getenv("BOUNDARY");

    std::string filename;
    size_t filename_start = body.find("filename=\"", 0);
    if (filename_start != std::string::npos) {
        filename_start += strlen("filename=\""); // Avancer jusqu'au début du nom de fichier
        size_t filename_end = body.find("\"", filename_start);
        if (filename_end != std::string::npos) {
            filename = body.substr(filename_start, filename_end - filename_start);
        }
    } else {
        std::cerr << "Filename not found in request" << std::endl;
        return 1;
    }

    // Trouver la position de départ des données du fichier
    size_t header_end = body.find("\r\n\r\n");
    std::cout << "header_end: " << header_end << std::endl;
    if (header_end == std::string::npos) {
        std::cerr << "Invalid file data format0" << std::endl;
        return 1;
    }
    header_end += 4;

    
    // Trouver la fin des données du fichier
    size_t boundary_end = body.find(boundary, header_end);
    std::cout << "boundary_end: " << boundary_end << std::endl;
    if (boundary_end == std::string::npos) {
        std::cerr << "Invalid file data format4" << std::endl;
        return 1;
    }
    boundary_end -= 4;

    // Vérifier si la requête est complète
    // if (boundary_end != body.length() - 4) {
    //     std::cerr << "Incomplete request" << std::endl;
    //     return 1;
    // }

    // Ouvrir le fichier de sortie
    std::ofstream outfile(("upload/" + filename).c_str(), std::ios::binary);
    if (!outfile.is_open()) {
        std::cerr << "Unable to save file: " << filename << std::endl;
        return 1;
    }

    // Écrire les données du fichier dans un nouveau fichier avec le nom dynamique
    size_t pos = header_end;
    while (pos < boundary_end) {
        size_t chunk_size = std::min(static_cast<size_t>(boundary_end - pos), static_cast<size_t>(1024 * 1024)); // Taille du chunk (1 Mo)
        outfile.write(body.data() + pos, chunk_size); // Écrire le chunk dans le fichier
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

    return 0;
}
