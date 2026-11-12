#include "mylib.h"
#include "avr_api.h"

/* Variables de temporización SysTick (1 ms) */
static volatile unsigned int tiempo_restante = 0;
static volatile unsigned char timer_activo = 0;
static volatile unsigned int contador_heartbeat_ms = 0;

/* Variables de estado interno del driver */
static unsigned int nivel_actual_l = 500;
static accion_recuperacion_t modo_recup_actual = RECUPERAR_REANUDAR;

/* Callback ejecutado cada 1 ms por interrupción de Timer0 */
static void isr_systick_1ms(void) {
    driver_tick_1ms();
    contador_heartbeat_ms++;
}

void init_driver(void) {
    /* 1. Salidas en PORTB:
     * PB0: Bomba | PB1: Válvula | PB2: Alarma | PB3: LED Heartbeat */
    GpioInitStructure_AVR salidas;
    salidas.port = avr_GPIO_B;
    salidas.modo = avr_GPIO_mode_Output;
    salidas.pines = avr_GPIO_PIN_0 | avr_GPIO_PIN_1 | avr_GPIO_PIN_2 | avr_GPIO_PIN_3;
    init_gpio(salidas);

    driver_bomba_off();
    driver_valvula_off();
    driver_alarma_off();
    clear_pin(avr_GPIOB_OUT, avr_GPIO_PIN_3);

    /* 2. Entradas en PORTD con Pull-up interno activado:
     * PD0: Paro Emergencia | PD1: Reset | PD2: Falla Bomba | PD3: Selector Modo */
    GpioInitStructure_AVR entradas;
    entradas.port = avr_GPIO_D;
    entradas.modo = avr_GPIO_mode_Input;
    entradas.pines = avr_GPIO_PIN_0 | avr_GPIO_PIN_1 | avr_GPIO_PIN_2 | avr_GPIO_PIN_3;
    init_gpio(entradas);

    /* Activar pull-ups internos en PD0..PD3 */
    set_pin(avr_GPIOD_OUT, avr_GPIO_PIN_0 | avr_GPIO_PIN_1 | avr_GPIO_PIN_2 | avr_GPIO_PIN_3);

    /* 3. Configuración ADC0 (PF0) en modo Free-Running (no bloqueante) */
    DDRF &= ~(1 << PF0);
    ADMUX = (1 << REFS0); /* Referencia AVCC (5V), canal ADC0 */
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADFR) |
             (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); /* Prescaler 128 */

    /* 4. Inicialización de SysTick cada 1 ms usando Timer0 de la API AVR */
    SystickInitStructure_AVR systick_cfg;
    systick_cfg.timernumber = avr_TIM0;
    systick_cfg.time_ms = 1;
    systick_cfg.avr_systick_handler = isr_systick_1ms;
    init_Systick_timer(systick_cfg);

    /* Habilitar interrupciones globales */
    sei();
}

/* --- Control de Actuadores --- */
void driver_bomba_on(void) {
    set_pin(avr_GPIOB_OUT, avr_GPIO_PIN_0);
}

void driver_bomba_off(void) {
    clear_pin(avr_GPIOB_OUT, avr_GPIO_PIN_0);
}

void driver_valvula_on(void) {
    set_pin(avr_GPIOB_OUT, avr_GPIO_PIN_1);
}

void driver_valvula_off(void) {
    clear_pin(avr_GPIOB_OUT, avr_GPIO_PIN_1);
}

void driver_alarma_on(void) {
    set_pin(avr_GPIOB_OUT, avr_GPIO_PIN_2);
}

void driver_alarma_off(void) {
    clear_pin(avr_GPIOB_OUT, avr_GPIO_PIN_2);
}

void driver_led_estado_toggle(void) {
    avr_GPIOB_OUT ^= avr_GPIO_PIN_3;
}

/* --- Lectura de Sensores (Sin bucles bloqueantes) --- */
unsigned int driver_leer_nivel_l(void) {
    unsigned int adc_val = ADC;
    nivel_actual_l = (unsigned int)(((unsigned long)adc_val * CAPACIDAD_TANQUE_L) / 1023UL);
    return nivel_actual_l;
}

unsigned char driver_paro_emergencia(void) {
    return (!avr_GPIOD_IN_0) ? 1 : 0;
}

unsigned char driver_falla_activa(void) {
    return (!avr_GPIOD_IN_2) ? 1 : 0;
}

unsigned char driver_reset_solicitado(void) {
    return (!avr_GPIOD_IN_1) ? 1 : 0;
}

accion_recuperacion_t driver_modo_recuperacion(void) {
    if (!avr_GPIOD_IN_3) {
        return RECUPERAR_VACIAR;
    }
    return modo_recup_actual;
}

/* --- Temporización No Bloqueante (SysTick 1 ms) --- */
void driver_iniciar_timer(unsigned int ms) {
    tiempo_restante = ms;
    timer_activo = 1;
}

unsigned char driver_timer_termino(void) {
    if (timer_activo && tiempo_restante == 0) {
        timer_activo = 0;
        return 1;
    }
    return 0;
}

void driver_tick_1ms(void) {
    if (timer_activo && tiempo_restante > 0) {
        tiempo_restante--;
    }
}

/* --- Tarea periódica no bloqueante (Heartbeat cada 500 ms) --- */
void driver_enviar_telemetria(estados_t estado_actual) {
    (void)estado_actual;
    if (contador_heartbeat_ms >= 500) {
        contador_heartbeat_ms = 0;
        driver_led_estado_toggle();
    }
}
