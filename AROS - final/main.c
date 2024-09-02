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
    for (int i = 0; i < 4; i++) {
        sem_post(&boat); 
        printf("Camac je dobio signal.\n");
    }
    for(int i = 0; i < 4; i++){
        sem_wait(&boat);
    }
}

void hacker_arrives() {
    sem_wait(&mutex);
    printf("Funkcija haker\n");
    hackersCount++;
    printf("Pristigao je haker, ukupan broj hakera je: %d\n", hackersCount);
    if (hackersCount >= 4) {
        int count = (hackersCount / 4) * 4;
        printf("Ukrcali su se 4 hakera.\n");
        hackersCount -= count;
        for (int i = 0; i < count - 1; i++) {
            sem_post(&hackerQueue);
        }
        sem_post(&mutex);
        cross_river();
    } else if (hackersCount >= 2 && serfsCount >= 2) {
        printf("Ukrcalo se 2 hakera i 2 radnika.\n");
        hackersCount -= 2;
        serfsCount -= 2;
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
    sem_post(&boat);
    return NULL;

}

void serf_arrives() {
    sem_wait(&mutex);
    printf("Funkcija radnik\n");
    serfsCount++;
    printf("Pristigao je radnik, ukupan broj radnika je: %d\n", serfsCount);
    if (serfsCount >= 4) {
        int count = (serfsCount / 4) * 4;
        printf("Ukrcalo se 4 radnika.\n");
        serfsCount -= count;
        for (int i = 0; i < count - 1; i++) {
            sem_post(&serfQueue);
        }
        sem_post(&mutex);
        cross_river();
    } else if (serfsCount >= 2 && hackersCount >= 2) {
        printf("Ukrcalo se 2 radnika i 2 hakera.\n");
        serfsCount -= 2;
        hackersCount -= 2;
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
    sem_post(&boat);
    return NULL;
}

int main() {
    sem_init(&mutex, 0, 1);
    sem_init(&hackerQueue, 0, 0);
    sem_init(&serfQueue, 0, 0);
    sem_init(&boat, 0, 0);

    pthread_t threads[8]; 
    for (int i = 0; i < 4; i++) { 
        pthread_create(&threads[i], NULL, hacker_arrives, NULL);
    }
    for (int i = 4; i < 8; i++) {
        pthread_create(&threads[i], NULL, serf_arrives, NULL);
    }

   
    for (int i = 0; i < 8; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&hackerQueue);
    sem_destroy(&serfQueue);
    sem_destroy(&boat);

    return 0;
}