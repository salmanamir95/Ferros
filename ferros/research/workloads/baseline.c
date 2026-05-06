#include <stdio.h>
#include <unistd.h>

/**
 * Baseline Workload
 * Represents: normal_baseline / normal_system_behavior
 * Logic: Minimal activity, long sleeps.
 */
int main() {
    printf("Starting Baseline Workload (Normal System Behavior Simulation)...\n");
    for (int i = 0; i < 30; i++) {
        sleep(1);
    }
    printf("Baseline Workload Complete.\n");
    return 0;
}
