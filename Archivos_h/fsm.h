#ifndef FSM_H
#define FSM_H

#include "sistema.h"

typedef estado_t (*funcion_estado_t)(sistema_t *sistema);

estado_t fsm_inicializar(sistema_t *sistema);
estado_t fsm_ejecutar(estado_t estado_actual, sistema_t *sistema);

#endif
