#include "hal_atmega128.h"
#include <avr/interrupt.h>

/* Variables compartidas con ISR */
static volatile bool g_flag_paro = false;
static volatile bool g_flag_reset = false;

/*
 * Configuracion de perifericos:
 * - PORTB: Salidas (Bomba, Valvula, Alarma, Heartbeat)
 * - PORTD: Entradas con pull-up (Paro, Reset, Falla bomba, Selector modo)
 * - ADC: Canal ADC0 (PF0), Vref = AVCC (5V), Prescaler = 128 (125 kHz @ 16 MHz)
 */
void hal_init(void)
{
    /* 1. Salidas en PORTB */
    DDRB |= (1 << PIN_ACTUADOR_BOMBA) |
            (1 << PIN_ACTUADOR_VALVULA) |
            (1 << PIN_ACTUADOR_ALARMA) |
            (1 << PIN_LED_ESTADO);

    PORTB &= ~((1 << PIN_ACTUADOR_BOMBA) |
               (1 << PIN_ACTUADOR_VALVULA) |
               (1 << PIN_ACTUADOR_ALARMA) |
               (1 << PIN_LED_ESTADO));

    /* 2. Entradas en PORTD con Pull-up activado */
    DDRD &= ~((1 << PIN_PARO_EMERGENCIA) |
              (1 << PIN_RESET_ALARMA) |
              (1 << PIN_FALLA_BOMBA) |
              (1 << PIN_MODO_RECUPERACION));

    PORTD |= (1 << PIN_PARO_EMERGENCIA) |
             (1 << PIN_RESET_ALARMA) |
             (1 << PIN_FALLA_BOMBA) |
             (1 << PIN_MODO_RECUPERACION);

    /* 3. Configuracion del ADC (ADC0 en PF0) */
    DDRF &= ~(1 << PF0);
    /* REFS0 = 1 (AVCC con capacitor en AREF), ADLAR = 0 (ajuste derecha) */
    ADMUX = (1 << REFS0);
    /* ADEN = 1 (habilitar ADC), ADPS2:0 = 7 (Prescaler 128) */
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    /* 4. Interrupciones externas opcionales (INT0: Paro, INT1: Reset) */
    /* Flanco descendente para INT0 e INT1 */
    EICRA |= (1 << ISC01) | (1 << ISC11);
    EIMSK |= (1 << INT0) | (1 << INT1);

    sei();
}

ISR(INT0_vect)
{
    g_flag_paro = true;
}

ISR(INT1_vect)
{
    g_flag_reset = true;
}

static uint16_t adc_leer(uint8_t canal)
{
    ADMUX = (ADMUX & 0xF8) | (canal & 0x07);
    ADCSRA |= (1 << ADSC);                     /* Iniciar conversion */
    while (ADCSRA & (1 << ADSC));              /* Esperar fin */
    return ADC;
}

void hal_leer_sensores(sistema_t *sistema)
{
    if (sistema == NULL) return;

    /* Lectura del nivel mediante ADC (0 a 1023 -> 0 a 1000 Litros) */
    uint16_t valor_adc = adc_leer(0);
    (*sistema).nivel_l = ((float)valor_adc * CAPACIDAD_TANQUE_L) / 1023.0f;

    /* Entradas digitales (logica invertida por pull-up interno) */
    bool pin_paro_presionado = !(PIND & (1 << PIN_PARO_EMERGENCIA));
    if (pin_paro_presionado || g_flag_paro) 
    {
        (*sistema).paro_emergencia = true;
        g_flag_paro = false;
    } 
    else 
    {
        /* Si se solto el paro mecanico */
        (*sistema).paro_emergencia = false;
    }

    bool pin_reset_presionado = !(PIND & (1 << PIN_RESET_ALARMA));
    if (pin_reset_presionado || g_flag_reset) 
    {
        (*sistema).reset_solicitado = true;
        g_flag_reset = false;
    }

    /* Sensor de falla de bomba (0 = Falla detectada por presostato/termico) */
    (*sistema).falla_activa = !(PIND & (1 << PIN_FALLA_BOMBA));

    /* Selector de modo de recuperacion */
    if (!(PIND & (1 << PIN_MODO_RECUPERACION))) 
    {
        (*sistema).accion_recuperacion = RECUPERAR_VACIAR;
    } 
    else 
    {
        (*sistema).accion_recuperacion = RECUPERAR_REANUDAR;
    }
}

void hal_actualizar_actuadores(const sistema_t *sistema)
{
    if (sistema == NULL) return;

    if ((*sistema).bomba_encendida) 
    {
        PORTB |= (1 << PIN_ACTUADOR_BOMBA);
    } 
    else 
    {
        PORTB &= ~(1 << PIN_ACTUADOR_BOMBA);
    }

    if ((*sistema).valvula_vaciado_abierta) 
    {
        PORTB |= (1 << PIN_ACTUADOR_VALVULA);
    } 
    else 
    {
        PORTB &= ~(1 << PIN_ACTUADOR_VALVULA);
    }

    if ((*sistema).alarma_activa) 
    {
        PORTB |= (1 << PIN_ACTUADOR_ALARMA);
    } 
    else 
    {
        PORTB &= ~(1 << PIN_ACTUADOR_ALARMA);
    }
}

void hal_toggle_heartbeat(void)
{
    PORTB ^= (1 << PIN_LED_ESTADO);
}
