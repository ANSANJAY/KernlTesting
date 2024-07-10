#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ALLOCATION_SIZE 1024 * 1024 // 1 MB
#define ALLOCATION_COUNT 10
#define SLEEP_TIME 1 // 1 second

void allocate_memory() {
    char *memory;
    for (int i = 0; i < ALLOCATION_COUNT; i++) {
        memory = (char *)malloc(ALLOCATION_SIZE);
        if (memory == NULL) {
            fprintf(stderr, "Memory allocation failed at iteration %d\n", i);
            exit(1);
        }
        memset(memory, 0, ALLOCATION_SIZE); // Use the allocated memory
        printf("Allocated %d MB\n", (i + 1));
        sleep(SLEEP_TIME); // Sleep to simulate long-running process
    }
}

int main() {
    allocate_memory();
    return 0;
}