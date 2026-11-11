#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

void uart0_init(uint32_t baudrate);
void uart0_transmit(char data);
void uart0_print(const char *str);
void uart0_print_float(float valor, uint8_t decimales);
void uart0_print_int(int32_t num);
bool uart0_char_available(void);
char uart0_receive(void);

#endif
