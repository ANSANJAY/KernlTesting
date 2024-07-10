#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define NUM_THREADS 4
#define NUM_REQUESTS 2
#define HOST "localhost"
#define PORT 8080
#define REQUEST "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"

// Very CPU-intensive task
void very_intensive_cpu_task() {
    for (volatile int i = 0; i < 1000000000; ++i); // Large loop for CPU load
}

// Moderately CPU-intensive task
void moderate_cpu_task() {
    for (volatile int i = 0; i < 10000000; ++i); // Moderate loop for CPU load
}

// Lightweight CPU task
void lightweight_cpu_task() {
    for (volatile int i = 0; i < 100000; ++i); // Small loop for CPU load
}

// Function to simulate file I/O
void file_io_task() {
    int fd = open("testfile.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("ERROR opening file");
        return;
    }
    char buffer[1024];
    memset(buffer, 'A', sizeof(buffer));
    for (int i = 0; i < 1000; ++i) {
        write(fd, buffer, sizeof(buffer));
    }
    close(fd);
}

// Function to simulate an HTTP request with a delay
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

    // Simulate network delay
    sleep(2);

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

// Thread function to run diverse tasks
void *diverse_tasks(void *arg) {
    for (int i = 0; i < NUM_REQUESTS; ++i) {
        very_intensive_cpu_task();
        moderate_cpu_task();
        lightweight_cpu_task();
        file_io_task();
        make_request();
    }
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, diverse_tasks, NULL);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
