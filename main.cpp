/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/23 16:09:27 by robin             #+#    #+#             */
/*   Updated: 2024/04/12 16:33:58 by robin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#define RESET   "\033[0m"
#define RED     "\033[31m"      // Rouge
#define GREEN   "\033[32m"      // Vert
#define YELLOW  "\033[33m"      // Jaune
#define BLUE    "\033[34m"      // Bleu
#define MAGENTA "\033[35m"      // Magenta
#define CYAN    "\033[36m"      // Cyan

#define MAX_EVENTS 64
#define PORT 8080

std::string readHttpRequest(int client_socket) {
    std::string request;
    char buffer[1000024];
    ssize_t bytes_read;

    // Lire les données du socket jusqu'à ce que la connexion soit fermée par le client
    while (true) {
        bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_read < 0) {
            std::cerr << "Error reading from socket" << std::endl;
            return "";
        } else if (bytes_read == 0) {
            std::cout << "Connection closed by client" << std::endl;
            break;
        }

        request.append(buffer, bytes_read);

        // Si la requête contient la séquence de fin "\r\n\r\n", cela signifie que la requête est complète
        size_t header_end = request.find("\r\n\r\n");
        if (header_end != std::string::npos) {
            std::cout << "Header fully received" << std::endl;
            // Vérifier si la requête contient le Content-Length
            size_t content_length_start = request.find("Content-Length: ");
            if (content_length_start != std::string::npos) {
                size_t content_length_end = request.find("\r\n", content_length_start);
                if (content_length_end != std::string::npos) {
                    std::string content_length_str = request.substr(content_length_start + strlen("Content-Length: "), content_length_end - content_length_start - strlen("Content-Length: "));
                    int content_length = std::stoi(content_length_str);
                    // Vérifier si le corps de la requête est entièrement reçu
                    if (request.size() >= header_end + 4 + content_length) {
                        std::cout << "Request fully received" << std::endl;
                        break;
                    }
                }
            } else {
                std::cout << "Request fully received" << std::endl;
                break;
            }
        }
    }

    std::cout << RED << request << RESET << std::endl;

    return request;
}



std::string readHtmlFile(const char *filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return ""; // Ou une erreur appropriée
    }

    std::string content;
    char ch;
    while (file.get(ch)) {
        content.push_back(ch);
    }
    file.close();
    return content;
}

void handleFileUpload(const std::string& request_body) {
    // Extraire le nom de fichier du Content-Disposition
    std::string filename;
    size_t filename_start = request_body.find("filename=\"", 0);
    if (filename_start != std::string::npos) {
        filename_start += strlen("filename=\""); // Avancer jusqu'au début du nom de fichier
        size_t filename_end = request_body.find("\"", filename_start);
        if (filename_end != std::string::npos) {
            filename = request_body.substr(filename_start, filename_end - filename_start);
        }
    } else {
        std::cerr << "Filename not found in request" << std::endl;
        return;
    }

    // Trouver la position de départ des données du fichier
size_t header_end = request_body.find("\r\n\r\n");
if (header_end == std::string::npos) {
    std::cerr << "Invalid file data format" << std::endl;
    return;
}

// Trouver la position du début du fichier en sautant trois lignes supplémentaires
size_t file_data_start = request_body.find("\r\n", header_end + 4);
if (file_data_start == std::string::npos) {
    std::cerr << "Invalid file data format" << std::endl;
    return;
}
file_data_start = request_body.find("\r\n", file_data_start + 2); // Sauter deux lignes supplémentaires
if (file_data_start == std::string::npos) {
    std::cerr << "Invalid file data format" << std::endl;
    return;
}
file_data_start = request_body.find("\r\n", file_data_start + 2); // Sauter deux lignes supplémentaires
if (file_data_start == std::string::npos) {
    std::cerr << "Invalid file data format" << std::endl;
    return;
}
file_data_start = request_body.find("\r\n", file_data_start + 2); // Sauter deux lignes supplémentaires
if (file_data_start == std::string::npos) {
    std::cerr << "Invalid file data format" << std::endl;
    return;
}
file_data_start += 2; // Avancer jusqu'au début des données du fichier

    
    // Trouver la fin des données du fichier
    size_t boundary_end = request_body.find("--------------", file_data_start);
    if (boundary_end == std::string::npos) {
        std::cerr << "Invalid file data format" << std::endl;
        return;
    }
    boundary_end -= 2;


    // Ouvrir le fichier de sortie
    std::ofstream outfile("upload/" + filename, std::ios::binary);
    if (!outfile.is_open()) {
        std::cerr << "Unable to save file: " << filename << std::endl;
        return;
    }

    // Écrire les données du fichier dans un nouveau fichier avec le nom dynamique
    size_t pos = file_data_start;
    while (pos < boundary_end) {
        size_t chunk_size = std::min(static_cast<size_t>(boundary_end - pos), static_cast<size_t>(1024 * 1024)); // Taille du chunk (1 Mo)
        outfile.write(request_body.data() + pos, chunk_size); // Écrire le chunk dans le fichier
        pos += chunk_size; // Avancer la position
    }

    // Fermer le fichier de sortie
    outfile.close();

    // Vérifier la taille du fichier
    std::ifstream infile("upload/" + filename, std::ios::binary | std::ios::ate);
    std::streamsize size = infile.tellg();
    infile.close();

    if (size == static_cast<std::streamsize>(boundary_end - file_data_start)) {
        std::cout << "File saved successfully: " << filename << std::endl;
    } else {
        std::cerr << "File size mismatch: " << filename << std::endl;
    }
}



int main() {
    int kq = kqueue();
    if (kq == -1) {
        perror("Error in kqueue");
        exit(EXIT_FAILURE);
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Error in socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error in bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) == -1) {
        perror("Error in listen");
        exit(EXIT_FAILURE);
    }

    struct kevent events[MAX_EVENTS];
    struct kevent change_event;
    EV_SET(&change_event, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);

    if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
        perror("Error in kevent");
        exit(EXIT_FAILURE);
    }

    std::cout << YELLOW << "Server started. Listening on port " << PORT << RESET << std::endl;

    while (true) {
        int num_events = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
        if (num_events == -1) {
            perror("Error in kevent");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < num_events; ++i) {
            if ((int)events[i].ident == server_fd) {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
                if (client_fd == -1) {
                    perror("Error in accept");
                    exit(EXIT_FAILURE);
                }

                if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
                    perror("Error in fcntl");
                    exit(EXIT_FAILURE);
                }

                if (fcntl(client_fd, F_SETFL, FD_CLOEXEC) == -1) {
                    perror("Error in fcntl");
                    exit(EXIT_FAILURE);
                }

                std::cout << GREEN << "New connection accepted" << RESET << std::endl;

                EV_SET(&change_event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
                    perror("Error in kevent");
                    exit(EXIT_FAILURE);
                }
            } else {
                int client_socket = events[i].ident;

                std::string httpRequestContent = readHttpRequest(client_socket);
                if (!httpRequestContent.empty()) {
                    if (httpRequestContent.find("POST /upload") != std::string::npos) {
                        handleFileUpload(httpRequestContent);
                        std::string htmlContent = readHtmlFile("socket.html");
                        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + htmlContent;

                        if (send(client_socket, response.c_str(), response.size(), 0) == -1) {
                            perror("Error in send");
                            close(client_socket);
                            return 1;
                        }
                        close(client_socket);
                        std::cout << BLUE << "Response sent." << RESET << std::endl;
                    } else {
                        std::string htmlContent = readHtmlFile("socket.html");
                        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + htmlContent;

                        if (send(client_socket, response.c_str(), response.size(), 0) == -1) {
                            perror("Error in send");
                            close(client_socket);
                            return 1;
                        }
                        close(client_socket);
                        std::cout << BLUE << "Response sent." << RESET << std::endl;
                    }
                }
            }
        }
    }
    close(server_fd);
    close(kq);
    return 0;
}
