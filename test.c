#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define NUM_THREADS 4
#define NUM_REQUESTS 5 // Reduced number of requests
#define HOST "localhost"
#define PORT 8080
#define REQUEST "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"

void waste_time() {
    for (volatile int i = 0; i < 1000000000; ++i); // Increased workload
}

void make_request() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[4096];
    ssize_t n;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        return;
    }

    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
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
    while ((n = read(sockfd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';  // Null-terminate the buffer
        // Process the response (not printed to avoid clutter)
    }

    if (n < 0) {
        perror("ERROR reading from socket");
    }

    // Close socket
    close(sockfd);
}

void *compute_and_request(void *arg) {
    for (int i = 0; i < NUM_REQUESTS; ++i) {
        waste_time();
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