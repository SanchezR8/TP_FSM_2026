#include <avr/io.h>
#include <util/delay.h>
#include "sistema.h"
#include "fsm.h"
#include "hal_atmega128.h"
#include "uart.h"

static void enviar_telemetria(const sistema_t *sistema, estado_t estado)
{
    uart0_print("[TANQUE] Estado: ");
    uart0_print(estado_nombre(estado));
    uart0_print(" | Nivel: ");
    uart0_print_float((*sistema).nivel_l, 1);
    uart0_print(" L (");
    uart0_print_float(((*sistema).nivel_l * 100.0f) / CAPACIDAD_TANQUE_L, 1);
    uart0_print("%) | Bomba: ");
    uart0_print((*sistema).bomba_encendida ? "ON" : "OFF");
    uart0_print(" | Valvula: ");
    uart0_print((*sistema).valvula_vaciado_abierta ? "ABIERTA" : "CERRADA");
    uart0_print(" | Alarma: ");
    uart0_print((*sistema).alarma_activa ? "ACTIVA" : "OK");
    uart0_print("\r\n");
}

static void atender_comandos_uart(sistema_t *sistema)
{
    if (uart0_char_available()) 
    {
        char cmd = uart0_receive();
        switch (cmd)
        {
            case 'p':
            case 'P':
                (*sistema).paro_emergencia = !(*sistema).paro_emergencia;
                uart0_print(">> COMANDO: Toggle Paro Emergencia\r\n");
                break;
            case 'r':
            case 'R':
                (*sistema).reset_solicitado = true;
                uart0_print(">> COMANDO: Reset Alarma Solicitado\r\n");
                break;
            case 'f':
            case 'F':
                (*sistema).falla_activa = !(*sistema).falla_activa;
                uart0_print(">> COMANDO: Toggle Falla Bomba\r\n");
                break;
            case 'm':
            case 'M':
                if ((*sistema).accion_recuperacion == RECUPERAR_REANUDAR) 
                {
                    (*sistema).accion_recuperacion = RECUPERAR_VACIAR;
                    uart0_print(">> MODO RECUPERACION: VACIAR\r\n");
                } 
                else 
                {
                    (*sistema).accion_recuperacion = RECUPERAR_REANUDAR;
                    uart0_print(">> MODO RECUPERACION: REANUDAR\r\n");
                }
                break;
            case '?':
            case 'h':
                uart0_print("--- MENU SERIAL ---\r\n");
                uart0_print("P: Paro Emergencia | R: Reset | F: Falla | M: Modo Recup\r\n");
                break;
            default:
                break;
        }
    }
}

int main(void)
{
    sistema_t sistema;
    estado_t estado_actual;
    uint16_t contador_ciclos = 0;

    /* 1. Inicializacion de Perifericos del ATmega128 */
    hal_init();
    uart0_init(9600);

    uart0_print("\r\n=========================================\r\n");
    uart0_print("  SISTEMA CONTROL TANQUE FSM - ATMEGA128 \r\n");
    uart0_print("=========================================\r\n");

    /* 2. Inicializacion de la logica del sistema y FSM */
    sistema_inicializar(&sistema, 0.0f);
    hal_leer_sensores(&sistema);
    estado_actual = fsm_inicializar(&sistema);

    /* 3. Bucle Principal de Control Embebido (Superloop) */
    while (1) 
    {
        /* Paso 1: Leer entradas fisicas (ADC, botones, pines) y comandos serial */
        hal_leer_sensores(&sistema);
        atender_comandos_uart(&sistema);

        /* Paso 2: Ejecutar la Maquina de Estados Finitos */
        estado_actual = fsm_ejecutar(estado_actual, &sistema);

        /* Paso 3: Aplicar enclavamientos de seguridad */
        sistema_aplicar_seguridad(&sistema);

        /* Paso 4: Actualizar salidas fisicas a los actuadores */
        hal_actualizar_actuadores(&sistema);

        /* Heartbeat y Telemetria periodica cada ~500 ms (50 ciclos x 10 ms) */
        contador_ciclos++;
        if (contador_ciclos >= 50) 
        {
            contador_ciclos = 0;
            hal_toggle_heartbeat();
            enviar_telemetria(&sistema, estado_actual);
        }

        /* Peroodo de muestreo del lazo de control: 10 ms */
        _delay_ms(10);
    }

    return 0;
}
