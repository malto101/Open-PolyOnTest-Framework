/**
 * Minimal Cortex-M33 startup for QEMU mps2-an505.
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>

extern uint32_t _estack;
extern uint32_t _sdata, _edata, _sidata;
extern uint32_t _sbss, _ebss;
extern void (*__init_array_start[])(void);
extern void (*__init_array_end[])(void);

void Reset_Handler(void);
void Default_Handler(void);
int main(void);

void Default_Handler(void) {
    for (;;) {
    }
}

static void call_constructors(void) {
    void (**fn)(void) = __init_array_start;
    while (fn < __init_array_end) {
        (*fn++)();
    }
}

void Reset_Handler(void) {
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }
    for (dst = &_sbss; dst < &_ebss; ++dst) {
        *dst = 0;
    }
    call_constructors();
    (void)main();
    for (;;) {
    }
}

__attribute__((section(".vectors"), used))
void (*const g_vectors[])(void) = {
    (void (*)(void))(uintptr_t)&_estack, /* Initial MSP */
    Reset_Handler,                       /* Reset */
    Default_Handler,                     /* NMI */
    Default_Handler,                     /* HardFault */
    Default_Handler,                     /* MemManage */
    Default_Handler,                     /* BusFault */
    Default_Handler,                     /* UsageFault */
    Default_Handler,                     /* SecureFault */
    0,
    0,
    0,
    Default_Handler, /* SVCall */
    Default_Handler, /* DebugMon */
    0,
    Default_Handler, /* PendSV */
    Default_Handler, /* SysTick */
};
