#ifndef BOARD_UART_H
#define BOARD_UART_H

#include <stddef.h>

void board_uart_init(void);
void board_uart_putc(char c);
void board_uart_write(const void *data, size_t len);

#endif
