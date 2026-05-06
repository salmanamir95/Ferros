#include <stdio.h>
#include <unistd.h>
#include <time.h>

/**
 * Burst Load Workload
 * Represents: cpu_bursty / bursty_resource_spike_behavior
 * Logic: Cycles of high intensity compute followed by sleep.
 */
void do_work(int iterations) {
    volatile unsigned long long counter = 0;
    for (int i = 0; i < iterations; i++) {
        counter++;
    }
}

int main() {
    printf("Starting Burst Load Workload (Bursty Resource Spike Analog)...\n");
    
    time_t start = time(NULL);
    while (time(NULL) - start < 30) {
        do_work(50000000); // High activity
        usleep(500000);    // 500ms Idle
    }
    
    printf("Burst Load Workload Complete.\n");
    return 0;
}
