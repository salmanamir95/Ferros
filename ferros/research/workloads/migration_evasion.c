#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <time.h>

/**
 * Migration Evasion Workload
 * Represents: migration_heavy / cpu_migration_intensive_behavior
 * Logic: Periodically pinning the process to different CPUs to force migration.
 */
int main() {
    printf("Starting Migration Evasion Workload (CPU Migration Intensive Analog)...\n");
    
    int num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    time_t start = time(NULL);
    int current_cpu = 0;
    
    while (time(NULL) - start < 30) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(current_cpu, &cpuset);
        
        if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) == 0) {
            // Do some work on this CPU
            volatile unsigned long long counter = 0;
            for (int i = 0; i < 5000000; i++) counter++;
        }
        
        current_cpu = (current_cpu + 1) % num_cpus;
        usleep(100000); // Shift every 100ms
    }
    
    printf("Migration Evasion Workload Complete.\n");
    return 0;
}
