#include <stdio.h>
#include <time.h>

/**
 * Sustained Load Workload
 * Represents: cpu_sustained / resource_exhaustion_behavior
 * Logic: High-intensity bounded compute loops.
 */
int main() {
    printf("Starting Sustained Load Workload (Resource Exhaustion Analog)...\n");
    
    time_t start = time(NULL);
    volatile unsigned long long counter = 0;
    
    // Bounded execution for exactly 30 seconds
    while (time(NULL) - start < 30) {
        for (int j = 0; j < 1000000; j++) {
            counter++; // Busy work
        }
    }
    
    printf("Sustained Load Workload Complete.\n");
    return 0;
}
