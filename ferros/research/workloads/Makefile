CC=gcc
CFLAGS=-O2 -pthread
TARGETS=baseline sustained_load burst_load scheduler_stress process_growth migration_evasion

all: $(TARGETS)

baseline: baseline.c
	$(CC) $(CFLAGS) -o baseline baseline.c

sustained_load: sustained_load.c
	$(CC) $(CFLAGS) -o sustained_load sustained_load.c

burst_load: burst_load.c
	$(CC) $(CFLAGS) -o burst_load burst_load.c

scheduler_stress: scheduler_stress.c
	$(CC) $(CFLAGS) -o scheduler_stress scheduler_stress.c

process_growth: process_growth.c
	$(CC) $(CFLAGS) -o process_growth process_growth.c

migration_evasion: migration_evasion.c
	$(CC) $(CFLAGS) -o migration_evasion migration_evasion.c

clean:
	rm -f $(TARGETS)
