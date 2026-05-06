#ifndef TYPES_H
#define TYPES_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned int __u32;
typedef unsigned long long __u64;
typedef int __s32;
typedef long long __s64;

typedef __u8 u8;
typedef __u16 u16;
typedef __u32 u32;
typedef __u64 u64;
typedef __s32 s32;
typedef __s64 s64;
#endif

#endif