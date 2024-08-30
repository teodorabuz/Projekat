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
    printf("Camac se sprema da se putnici ukrcaju.\n");
    sem_wait(&mutex);
    for (int i = 0; i < 4; i++) {
        sem_post(&boat); 
        printf("Camac je dobio signal.\n");
    }
    sem_post(&mutex);
    sem_wait(&mutex);
    for(int i = 0; i < 4; i++){
        sem_wait(&boat);
    }
    sem_post(&mutex);
}

void hacker_arrives() {
    sem_wait(&mutex);
    printf("Funkcija haker\n");
    hackersCount++;
    printf("Pristigao je haker, ukupan broj hakera je: %d\n", hackersCount);
    if (hackersCount >= 4) {
        int count = (hackersCount / 4) * 4;
        printf("Ukrcali su se %d hakera.\n", hackersCount);
        hackersCount -= count;
        for (int i = 0; i < count; i++) {
            sem_post(&hackerQueue);
        }
        sem_post(&mutex);
        cross_river();
    } else if (hackersCount >= 2 && serfsCount >= 2) {
        printf("Ukrcalo se %d hakera i %d radnika.\n", hackersCount, serfsCount);
        hackersCount -= 2;
        serfsCount -= 2;
        sem_post(&hackerQueue);
        sem_post(&hackerQueue);
        sem_post(&serfQueue);
        sem_post(&serfQueue);
        sem_post(&mutex);
        cross_river();
    } else {
        sem_post(&mutex);
        printf("Cekanje u funkciji haker.\n");
        sem_wait(&hackerQueue); 
    }
    sem_wait(&boat);
    return NULL;

}

void serf_arrives() {
    sem_wait(&mutex);
    printf("Funkcija radnik\n");
    serfsCount++;
    printf("Pristigao je radnik, ukupan broj radnika je: %d\n", serfsCount);
    if (serfsCount >= 4) {
        int count = (serfsCount / 4) * 4;
        printf("Ukrcalo se %d radnika.\n",serfsCount);
        serfsCount -= count;
        for (int i = 0; i < count; i++) {
            sem_post(&serfQueue);
        }
        sem_post(&mutex);
        cross_river();
    } else if (serfsCount >= 2 && hackersCount >= 2) {
        printf("Ukrcalo se %d radnika i %d hakera.\n", serfsCount, hackersCount);
        serfsCount -= 2;
        hackersCount -= 2;
        sem_post(&serfQueue);
        sem_post(&serfQueue);
        sem_post(&hackerQueue);
        sem_post(&hackerQueue);
        sem_post(&mutex);
        cross_river();
    } else {
        sem_post(&mutex);
        printf("Cekanje u funkciji radnik.\n");
        sem_wait(&serfQueue); 
    }
    sem_wait(&boat);
    return NULL;
}

void board_boat() {
    sem_wait(&boat); 
    printf("Boat is crossing the river.\n");
    sem_post(&mutex); 
}

int main() {
    sem_init(&mutex, 0, 1);
    sem_init(&hackerQueue, 0, 0);
    sem_init(&serfQueue, 0, 0);
    sem_init(&boat, 0, 0);

    pthread_t threads[10]; 
    for (int i = 0; i < 5; i++) { 
        pthread_create(&threads[i], NULL, hacker_arrives, NULL);
    }
    for (int i = 5; i < 10; i++) {
        pthread_create(&threads[i], NULL, serf_arrives, NULL);
    }

   
    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&hackerQueue);
    sem_destroy(&serfQueue);
    sem_destroy(&boat);

    return 0;
}