/**
 * Semihosting character I/O for QEMU (works without UART PPC setup).
 * SPDX-License-Identifier: Apache-2.0
 */
#include "board_uart.h"

#include <stddef.h>
#include <stdint.h>

void board_uart_init(void) {
    /* no-op for semihosting path */
}

static void sh_writec(char c) {
    register uint32_t r0 __asm("r0") = 0x03u; /* SYS_WRITEC */
    register uint32_t r1 __asm("r1") = (uint32_t)(uintptr_t)&c;
    __asm volatile("bkpt #0xAB" : : "r"(r0), "r"(r1) : "memory");
}

void board_uart_putc(char c) { sh_writec(c); }

void board_uart_write(const void *data, size_t len) {
    const char *p = (const char *)data;
    for (size_t i = 0; i < len; ++i) {
        sh_writec(p[i]);
    }
}
