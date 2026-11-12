#ifndef MYLIB_H
#define MYLIB_H

#include "avr_api.h"

/* Parámetros del tanque */
#define CAPACIDAD_TANQUE_L     1000
#define NIVEL_INICIO_LLENADO   250   /* 25% */
#define NIVEL_FIN_LLENADO      900   /* 90% */
#define NIVEL_CRITICO          950   /* 95% */

/* Estados de la Máquina de Estados Finitos (FSM) */
typedef enum {
    INICIO = 0,
    ESPERA,
    LLENADO,
    ALARMA,
    RECUPERACION,
    VACIANDO,
    CANTIDAD_ESTADOS
} estados_t;

/* Modos de recuperación ante alarma */
typedef enum {
    RECUPERAR_REANUDAR = 1,
    RECUPERAR_REINICIAR,
    RECUPERAR_VACIAR
} accion_recuperacion_t;

/* Inicialización del hardware y sistema */
void init_driver(void);

/* Prototipos de las funciones de estado (FSM) */
estados_t f_inicio(void);
estados_t f_espera(void);
estados_t f_llenado(void);
estados_t f_alarma(void);
estados_t f_recuperacion(void);
estados_t f_vaciando(void);

/* Funciones de control de actuadores (Salidas) */
void driver_bomba_on(void);
void driver_bomba_off(void);
void driver_valvula_on(void);
void driver_valvula_off(void);
void driver_alarma_on(void);
void driver_alarma_off(void);
void driver_led_estado_toggle(void);

/* Funciones de lectura de sensores (Entradas) */
unsigned int driver_leer_nivel_l(void);
unsigned char driver_paro_emergencia(void);
unsigned char driver_falla_activa(void);
unsigned char driver_reset_solicitado(void);
accion_recuperacion_t driver_modo_recuperacion(void);

/* Funciones de temporización no bloqueante (SysTick 1ms) y tarea periódica */
void driver_iniciar_timer(unsigned int ms);
unsigned char driver_timer_termino(void);
void driver_tick_1ms(void);
void driver_enviar_telemetria(estados_t estado_actual);

#endif
