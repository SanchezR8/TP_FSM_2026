#include <stddef.h>
#include "fsm.h"
#include "estados.h"

static funcion_estado_t tabla_fsm[CANTIDAD_ESTADOS] = 
{
    f_espera,
    f_llenado,
    f_alarma,
    f_recuperacion,
    f_vaciando
};

estado_t fsm_inicializar(sistema_t *sistema)
{
    return f_inicio(sistema);
}

estado_t fsm_ejecutar(estado_t estado_actual, sistema_t *sistema)
{
    if (sistema == NULL) return ALARMA;

    if (estado_actual >= CANTIDAD_ESTADOS) 
    {
        return ALARMA;
    }

    return tabla_fsm[estado_actual](sistema);
}
