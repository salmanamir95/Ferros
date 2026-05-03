#ifndef TYPES_H
#define TYPES_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
typedef unsigned int __u32;
typedef unsigned long long __u64;

typedef __u32 u32;
typedef __u64 u64;
#endif

#endif