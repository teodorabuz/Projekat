#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

sem_t mutex, hackerQueue, serfQueue, boat;
int hackersCount = 0, serfsCount = 0;

void cross_river(void) {
    // This function is called by each group of four after forming
    for (int i = 0; i < 4; i++) {
        sem_post(&boat); // Each member signals that the boat can cross
    }
}

void hacker_arrives() {
    sem_wait(&mutex);
    hackersCount++;
    if (hackersCount == 4) {
        // Four hackers form a group
        sem_post(&hackerQueue); sem_post(&hackerQueue); sem_post(&hackerQueue);
        hackersCount -= 4;
        cross_river();
    } else if (hackersCount == 2 && serfsCount >= 2) {
        // Two hackers and two serfs form a group
        sem_post(&hackerQueue); 
        sem_post(&serfQueue); sem_post(&serfQueue);
        hackersCount -= 2;
        serfsCount -= 2;
        cross_river();
    } else {
        sem_post(&mutex);
        sem_wait(&hackerQueue); // Hacker waits for a group
    }
}

void serf_arrives() {
    sem_wait(&mutex);
    serfsCount++;
    if (serfsCount == 4) {
        // Four serfs form a group
        sem_post(&serfQueue); sem_post(&serfQueue); sem_post(&serfQueue);
        serfsCount -= 4;
        cross_river();
    } else if (serfsCount == 2 && hackersCount >= 2) {
        // Two serfs and two hackers form a group
        sem_post(&serfQueue);
        sem_post(&hackerQueue); sem_post(&hackerQueue);
        serfsCount -= 2;
        hackersCount -= 2;
        cross_river();
    } else {
        sem_post(&mutex);
        sem_wait(&serfQueue); // Serf waits for a group
    }
}

void board_boat() {
    sem_wait(&boat); // Wait for the boat to be ready
    // Perform crossing logic (e.g., print a message)
    printf("Boat is crossing the river.\n");
    sem_post(&mutex); // Allow new groups to form after crossing
}

int main() {
    sem_init(&mutex, 0, 1);
    sem_init(&hackerQueue, 0, 0);
    sem_init(&serfQueue, 0, 0);
    sem_init(&boat, 0, 0);

    pthread_t threads[10]; // Example: 10 threads, could be more
    // Create threads representing hackers and serfs
    for (int i = 0; i < 5; i++) { // 5 hackers
        pthread_create(&threads[i], NULL, (void*)hacker_arrives, NULL);
    }
    for (int i = 5; i < 10; i++) { // 5 serfs
        pthread_create(&threads[i], NULL, (void*)serf_arrives, NULL);
    }

    // Join threads
    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}