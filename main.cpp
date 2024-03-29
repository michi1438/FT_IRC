/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/23 16:09:27 by robin             #+#    #+#             */
/*   Updated: 2024/03/28 14:55:10 by robin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <fstream>
#include <vector>
#include <poll.h> // Pour poll()

#define RED     "\033[31m"
#define RESET   "\033[0m"
#define RED     "\033[31m"      // Rouge
#define GREEN   "\033[32m"      // Vert
#define YELLOW  "\033[33m"      // Jaune
#define BLUE    "\033[34m"      // Bleu
#define MAGENTA "\033[35m"      // Magenta
#define CYAN    "\033[36m"      // Cyan

std::string readHtmlFile(const char *filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return ""; // Ou une erreur appropriée
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    return content;
}

int main() {
    // Création du socket
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == -1) {
        perror("socket");
        return 1;
    }

    // Configuration de l'adresse du serveur
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8000); // Port sur lequel le serveur écoute
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Liaison du socket à l'adresse et au port spécifiés
    if (bind(listener, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(listener);
        return 1;
    }

    // Mise en mode écoute du socket
    if (listen(listener, SOMAXCONN) == -1) {
        perror("listen");
        close(listener);
        return 1;
    }

    std::cout << YELLOW << "Server listening on port 8000 ..." << RESET << std::endl;

    std::vector<pollfd> fds(1); // Initialise le tableau de pollfd avec une seule entrée pour le descripteur de fichier du socket d'écoute
    fds[0].fd = listener;
    fds[0].events = POLLIN;

    while (true) {
        int pollResult = poll(fds.data(), fds.size(), -1); // Attend indéfiniment les événements d'E/S

        if (pollResult == -1) {
            perror("poll");
            return 1;
        }

        for (size_t i = 0; i < fds.size(); ++i) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == listener) {
                    // Nouvelle connexion entrante
                    int client_socket = accept(listener, NULL, NULL);
                    if (client_socket == -1) {
                        perror("accept");
                        return 1;
                    }
                    
                    // Ajouter une nouvelle entrée à fds
                    pollfd new_fd;
                    new_fd.fd = client_socket;
                    new_fd.events = POLLIN | POLLOUT;
                    fds.push_back(new_fd);
                } else {
                    // Connexion existante a des données à lire
                    int client_socket = fds[i].fd;
                    std::cout << GREEN << "Client connected." << RESET << std::endl;

                    // Réponse HTTP avec le contenu de socket.html
                    std::string htmlContent = readHtmlFile("socket.html");
                    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + htmlContent;

                    if (send(client_socket, response.c_str(), response.size(), 0) == -1) {
                        perror("send");
                        close(client_socket);
                        return 1;
                    }

                    // Ferme le socket client
                    close(client_socket);
                    std::cout << BLUE << "Response sent." << RESET << std::endl;

                    // Supprime le descripteur de fichier du tableau fds
                    fds.erase(fds.begin() + i);
                }
            }
        }
    }

    return 0;
}


