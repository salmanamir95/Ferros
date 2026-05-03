#include <map>
#include <string>
#include "types.h"
#include "events.h"

std::map<u32, std::string> process_table;

void ingest(const cpu_event &e) {
    process_table[e.pid] = e.comm;
}