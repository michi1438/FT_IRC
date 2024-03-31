#include <sys/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#define MAX_EVENTS 64
#define PORT 8080

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

    std::cout << "Server started. Listening on port " << PORT << std::endl;

    while (true) {
        int num_events = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
        if (num_events == -1) {
            perror("Error in kevent");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < num_events; ++i) {
            if (events[i].ident == server_fd) {
                // Accept new connection
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
                if (client_fd == -1) {
                    perror("Error in accept");
                    exit(EXIT_FAILURE);
                }

                // Set client socket to non-blocking
				if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
				{
                    perror("Error in fcntl");
                    exit(EXIT_FAILURE);
                }

				if (fcntl(client_fd, F_SETFL, FD_CLOEXEC) == -1)
				{
                    perror("Error in fcntl");
                    exit(EXIT_FAILURE);
                }

                std::cout << "New connection accepted" << std::endl;

                // Add client socket to kqueue
                EV_SET(&change_event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
                    perror("Error in kevent");
                    exit(EXIT_FAILURE);
                }
            } else {
                // Handle client request
				char response_header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
				send(events[i].ident, response_header, strlen(response_header), 0);

				FILE *file = fopen("index.html", "r");
				if (file == NULL) {
					perror("Error opening file");
					exit(EXIT_FAILURE);
				}

				char buffer[1024];
				size_t bytes_read;
				while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
					send(events[i].ident, buffer, bytes_read, 0);
				}
				fclose(file);
//				close(events[i].ident);
            }
        }
    }
    close(server_fd);
    close(kq);

    return 0;
}
