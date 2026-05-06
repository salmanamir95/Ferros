#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

/**
 * Process Growth Workload
 * Represents: process_tree_growth / hierarchical_process_growth_behavior
 * Logic: Controlled recursive spawning to a fixed depth.
 */
void spawn_children(int depth) {
    if (depth <= 0) {
        sleep(20); // Keep leaf processes alive for a bit
        return;
    }
    
    pid_t p1 = fork();
    if (p1 == 0) {
        spawn_children(depth - 1);
        _exit(0);
    }
    
    pid_t p2 = fork();
    if (p2 == 0) {
        spawn_children(depth - 1);
        _exit(0);
    }
    
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
}

int main() {
    printf("Starting Process Growth Workload (Hierarchical Process Growth Analog)...\n");
    
    // Max depth 4 = 2^4 - 1 children (15 children)
    spawn_children(3);
    
    printf("Process Growth Workload Complete.\n");
    return 0;
}
