#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#define NUM_THREADS 4
#define NUM_REQUESTS 10
#define HOST "example.com"
#define PORT 80
#define REQUEST "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n"

void waste_time() {
    for (volatile int i = 0; i < 100000000; ++i);
}

void make_request() {
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[4096];
    
    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        return;
    }
    
    // Get server by name
    server = gethostbyname(HOST);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        close(sockfd);
        return;
    }
    
    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(PORT);
    
    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR connecting");
        close(sockfd);
        return;
    }
    
    // Send request
    if (write(sockfd, REQUEST, strlen(REQUEST)) < 0) {
        perror("ERROR writing to socket");
        close(sockfd);
        return;
    }
    
    // Read response
    while (read(sockfd, buffer, sizeof(buffer) - 1) > 0) {
        // Process response (for demonstration, we just print it)
        // In a real scenario, you would process the response data
    }
    
    // Close socket
    close(sockfd);
}

void *compute_and_request(void *arg) {
    waste_time();
    for (int i = 0; i < NUM_REQUESTS; ++i) {
        make_request();
    }
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, compute_and_request, NULL);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}