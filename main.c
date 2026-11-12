#include "mylib.h"

int main(void) {
    init_driver();
    estados_t estado = INICIO;

    /* Vector de punteros a función para la FSM */
    estados_t (*fsm[CANTIDAD_ESTADOS])(void) = 
    {
        f_inicio,
        f_espera,
        f_llenado,
        f_alarma,
        f_recuperacion,
        f_vaciando
    };

    /* Único bucle infinito del sistema */
    while (1) 
    {
        estado = (*fsm[estado])();
        driver_enviar_telemetria(estado);
    }

    return 0;
}
