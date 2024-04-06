/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/23 16:09:27 by robin             #+#    #+#             */
/*   Updated: 2024/04/06 12:11:12 by robin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
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
    char buffer[1024];
    ssize_t bytes_read;

    // Lire les données du socket jusqu'à ce que la requête soit complètement reçue
    while ((bytes_read = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        std::cout << "Bytes read: " << bytes_read << std::endl;
        request.append(buffer, bytes_read);
        std::cout << "Request so far:\n" << request << std::endl;
        if (request.find("\r\n\r\n") != std::string::npos) {
            // L'en-tête complet de la requête a été reçu
            std::cout << "Header fully received\n";
            break;
        }
    }

    std::cout << "Final request:\n" << request << std::endl;

    // Vérifier si la requête est une requête POST et si elle contient des données de formulaire multipart
    size_t post_index = request.find("POST");
    if (post_index != std::string::npos) {
        size_t content_type_index = request.find("Content-Type: multipart/form-data", post_index);
        if (content_type_index != std::string::npos) {
            // La requête est une requête POST avec des données de formulaire multipart
            std::cout << "File upload request received\n";
            return request;
        }
    }

    // La requête n'est pas une requête POST avec des données de formulaire multipart
    return "";
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

void handleFileUpload(int client_socket, const std::string& request_body) {
    (void)client_socket;
    std::cout << YELLOW << request_body << RESET << std::endl;
    std::cout << RED << "Handling file upload request" << RESET << std::endl;

    // Trouver la position de début des données du fichier
    size_t data_start = request_body.find("\r\n\r\n");
    if (data_start == std::string::npos) {
        std::cerr << "Invalid file data format" << std::endl;
        return;
    }
    data_start += 4; // Passer les caractères de séparation
    std::cout << "Data start position: " << data_start << std::endl;

    // Extraire les données du fichier
    std::string file_data = request_body.substr(data_start);

    // Extraire le nom du fichier du Content-Disposition
    std::istringstream iss(request_body);
    std::string line;
    std::string filename;
    while (std::getline(iss, line)) {
        if (line.find("Content-Disposition: form-data; name=\"fileInput\";") != std::string::npos) {
            // Trouver le début du nom de fichier
            size_t filename_start = line.find("filename=\"");
            if (filename_start != std::string::npos) {
                filename_start += 10; // Avancer jusqu'au début du nom de fichier
                // Trouver la fin du nom de fichier
                size_t filename_end = line.find("\"", filename_start);
                if (filename_end != std::string::npos) {
                    filename = line.substr(filename_start, filename_end - filename_start);
                    break;
                }
            }
        }
    }

    // Écrire les données du fichier dans un nouveau fichier avec le nom dynamique
    if (!filename.empty()) {
        std::ofstream outfile("upload/" + filename, std::ios::binary);
        if (outfile.is_open()) {
            outfile << file_data;
            outfile.close();
            std::cout << "File saved successfully: " << filename << std::endl;
        } else {
            std::cerr << "Unable to save file: " << filename << std::endl;
        }
    } else {
        std::cerr << "Filename not found in request" << std::endl;
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
                // Accept new connection
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
                if (client_fd == -1) {
                    perror("Error in accept");
                    exit(EXIT_FAILURE);
                }

                // Set client socket to non-blocking
                if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
                    perror("Error in fcntl");
                    exit(EXIT_FAILURE);
                }

                if (fcntl(client_fd, F_SETFL, FD_CLOEXEC) == -1) {
                    perror("Error in fcntl");
                    exit(EXIT_FAILURE);
                }

                std::cout << GREEN << "New connection accepted" << RESET << std::endl;

                // Add client socket to kqueue
                EV_SET(&change_event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
                    perror("Error in kevent");
                    exit(EXIT_FAILURE);
                }
            } else {
                int client_socket = events[i].ident;

                // Lire la requête HTTP pour l'upload de fichier
                std::string httpRequestContent = readHttpRequest(client_socket);
                if (!httpRequestContent.empty()) {
                    std::cout << "File upload request received\n";
                    handleFileUpload(client_socket, httpRequestContent);
                }

                // Réponse HTTP avec le contenu de socket.html
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
    close(server_fd);
    close(kq);
    return 0;
}
