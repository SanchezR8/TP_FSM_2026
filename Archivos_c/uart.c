#include "uart.h"
#include <avr/io.h>
#include <stdio.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

void uart0_init(uint32_t baudrate)
{
    uint16_t ubrr = (F_CPU / (16UL * baudrate)) - 1;
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    /* Habilitar Transmisor y Receptor */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    /* Formato de trama: 8 bits de datos, 1 bit de stop, sin paridad */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart0_transmit(char data)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void uart0_print(const char *str)
{
    while (*str) 
    {
        uart0_transmit(*str++);
    }
}

void uart0_print_int(int32_t num)
{
    char buf[12];
    snprintf(buf, sizeof(buf), "%ld", (long)num);
    uart0_print(buf);
}

void uart0_print_float(float valor, uint8_t decimales)
{
    char buf[16];
    int32_t entero = (int32_t)valor;
    float fraccion = valor - (float)entero;
    if (fraccion < 0) fraccion = -fraccion;

    int32_t factor = 1;
    for (uint8_t i = 0; i < decimales; i++) 
    {
        factor *= 10;
    }
    int32_t dec = (int32_t)(fraccion * factor + 0.5f);

    snprintf(buf, sizeof(buf), "%ld.%0*ld", (long)entero, decimales, (long)dec);
    uart0_print(buf);
}

bool uart0_char_available(void)
{
    return (UCSR0A & (1 << RXC0)) != 0;
}

char uart0_receive(void)
{
    while (!uart0_char_available());
    return UDR0;
}
