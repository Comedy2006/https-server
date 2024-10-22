#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 4096

int main() {
    int sockfd, client_sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    
    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
    server_addr.sin_port = htons(8080);        // Port number
    
    // Bind the socket to the address and port
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(1);
    }
    
    // Listen for incoming connections
    if (listen(sockfd, 5) < 0) {
        perror("listen");
        close(sockfd);
        exit(1);
    }
    
    printf("Server is listening on port 8080...\n");

    // Accept incoming connections
    while (1) {
        client_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (client_sockfd < 0) {
            perror("accept");
            close(sockfd);
            exit(1);
        }

        // Clear the buffer and read the HTTP request
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_sockfd, buffer, BUFFER_SIZE - 1, 0);

        // Log the request for debugging purposes
        printf("Received request:\n%s\n", buffer);
        
        // Check the requested path in the HTTP request
        if (strncmp(buffer, "GET /example ", 13) == 0) {
            
            const char *body = "<h1>localhost:8080/example Page</h1>";
            int content_length = strlen(body);

            char response[BUFFER_SIZE];

            snprintf(response, sizeof(response),
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: %d\r\n"
                "\r\n"
                "%s",
                content_length, body);

            send(client_sockfd, response, sizeof(response) - 1, 0);
        } 
        else if (strncmp(buffer, "GET / ", 6) == 0) {
            const char *body = "Main Page";
            int content_length = strlen(body);

            char response[BUFFER_SIZE];

            snprintf(response, sizeof(response),
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: %d\r\n"
                "\r\n"
                "%s",
                content_length, body);

            send(client_sockfd, response, sizeof(response) - 1, 0);
        } 
        else if (strncmp(buffer, "GET /shutdown ", 14) == 0) {
            const char *body = "Server Shutting Down!";
            int content_length = strlen(body);

            char response[BUFFER_SIZE];

            snprintf(response, sizeof(response),
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: %d\r\n"
                "\r\n"
                "%s",
                content_length, body);
            send(client_sockfd, response, sizeof(response) - 1, 0);

            close(client_sockfd);
            break;
        }
        else {
            const char *body = "404 Not Found";
            int content_length = strlen(body);

            char response[BUFFER_SIZE];
            snprintf(response, sizeof(response),
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: %d\r\n"
                "\r\n"
                "%s",
                content_length, body);
            send(client_sockfd, response, sizeof(response) - 1, 0);
        }
        
        // Close the client connection
        close(client_sockfd);
    }
    
    // Close the server socket (never reached in this case)
    close(sockfd);
    
    return 0;
}
