#ifndef HAL_ATMEGA128_H
#define HAL_ATMEGA128_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include "sistema.h"

/*
 * MAPEO DE PINES Y PERIFERICOS (ATmega128):
 *
 * 1. ENTRADAS DIGITALES (Pines con pull-up interno en PORTD):
 *    - PD0 (INT0): Pulsador Paro de Emergencia (NC o activo en bajo / pulso)
 *    - PD1 (INT1): Pulsador Reset de Alarma (activo en bajo)
 *    - PD2 (INT2): Sensor/Interruptor Falla de Bomba (activo en bajo)
 *    - PD3: Selector Accion Recuperacion (Bajo: Reanudar, Alto: Vaciar)
 *
 * 2. SALIDAS DIGITALES (PORTB):
 *    - PB0: Rele / Actuador Bomba de llenado
 *    - PB1: Rele / Valvula de vaciado / drenaje
 *    - PB2: Sirena / Luz de Alarma
 *    - PB3: LED indicador de Estado / Heartbeat
 *
 * 3. ENTRADA ANALOGICA (ADC):
 *    - PF0 (ADC0): Sensor de nivel continuo (0V = 0 L, 5V = 1000 L)
 *
 * 4. COMUNICACION SERIE (USART0):
 *    - PE0 (RXD0) / PE1 (TXD0): Telemetria y consola a 9600 baudios
 */
 
#define PIN_PARO_EMERGENCIA   PD0
#define PIN_RESET_ALARMA      PD1
#define PIN_FALLA_BOMBA       PD2
#define PIN_MODO_RECUPERACION PD3

#define PIN_ACTUADOR_BOMBA    PB0
#define PIN_ACTUADOR_VALVULA  PB1
#define PIN_ACTUADOR_ALARMA   PB2
#define PIN_LED_ESTADO        PB3

void hal_init(void);
void hal_leer_sensores(sistema_t *sistema);
void hal_actualizar_actuadores(const sistema_t *sistema);
void hal_toggle_heartbeat(void);

#endif
