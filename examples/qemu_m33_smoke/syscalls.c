/**
 * Minimal freestanding stubs — no newlib required.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "board_uart.h"

#include <stddef.h>
#include <stdint.h>

extern char _ebss;
static char *heap_end;

void *memcpy(void *dst, const void *src, size_t n) {
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    while (n--) {
        *d++ = *s++;
    }
    return dst;
}

void *memset(void *dst, int c, size_t n) {
    unsigned char *d = (unsigned char *)dst;
    while (n--) {
        *d++ = (unsigned char)c;
    }
    return dst;
}

size_t strlen(const char *s) {
    size_t n = 0;
    while (s[n]) {
        ++n;
    }
    return n;
}

int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) {
        ++a;
        ++b;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

int strncmp(const char *a, const char *b, size_t n) {
    while (n && *a && (*a == *b)) {
        ++a;
        ++b;
        --n;
    }
    if (n == 0) {
        return 0;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

int memcmp(const void *pa, const void *pb, size_t n) {
    const unsigned char *a = (const unsigned char *)pa;
    const unsigned char *b = (const unsigned char *)pb;
    while (n--) {
        if (*a != *b) {
            return *a - *b;
        }
        ++a;
        ++b;
    }
    return 0;
}

void *_sbrk(int incr) {
    if (heap_end == 0) {
        heap_end = &_ebss;
    }
    char *prev = heap_end;
    heap_end += incr;
    return prev;
}

void board_qemu_exit(int code) {
    uint32_t report = (code == 0) ? 0x20026u : (uint32_t)code;
    register uint32_t r0 __asm("r0") = 0x18u;
    register uint32_t r1 __asm("r1") = report;
    __asm volatile("bkpt #0xAB" : : "r"(r0), "r"(r1) : "memory");
    for (;;) {
    }
}

void _exit(int code) { board_qemu_exit(code); }

/* Called by some libgcc/personality paths; keep linked. */
void abort(void) {
    board_qemu_exit(1);
}
