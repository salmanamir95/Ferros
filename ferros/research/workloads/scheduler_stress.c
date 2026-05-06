#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <time.h>

#define THREAD_COUNT 16

/**
 * Scheduler Stress Workload
 * Represents: scheduler_pressure / scheduler_pressure_behavior
 * Logic: Many threads performing frequent yields and minimal work.
 */
void* stress_worker(void* arg) {
    time_t start = time(NULL);
    while (time(NULL) - start < 30) {
        for (int i = 0; i < 100; i++) {
            sched_yield(); // Voluntarily give up CPU
        }
    }
    return NULL;
}

int main() {
    printf("Starting Scheduler Stress Workload (Scheduler Pressure Analog)...\n");
    
    pthread_t threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, stress_worker, NULL);
    }
    
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("Scheduler Stress Workload Complete.\n");
    return 0;
}
