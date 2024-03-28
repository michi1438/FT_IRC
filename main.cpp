/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/23 16:09:27 by robin             #+#    #+#             */
/*   Updated: 2024/03/28 13:09:12 by robin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // Pour close()
#include <cstring> // Pour memset()
#include <string>
#include <fstream>

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

    std::cout << "Server listening on port 8000..." << std::endl;

    while (true) {
        // Accepte une connexion entrante
        int client_socket = accept(listener, NULL, NULL);
        if (client_socket == -1) {
            perror("accept");
            close(listener);
            return 1;
        }

        std::cout << "Client connected." << std::endl;

        // Réponse HTTP avec le contenu "Hello!"
        std::string htmlContent = readHtmlFile("socket.html");

        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + htmlContent;

        if (send(client_socket, response.c_str(), response.size(), 0) == -1) {
            perror("send");
            close(client_socket);
            close(listener);
            return 1;
        }

        // Ferme le socket client
        close(client_socket);
        std::cout << "Response sent." << std::endl;
    }

    // Ferme le socket d'écoute
    close(listener);
    return 0;
}

