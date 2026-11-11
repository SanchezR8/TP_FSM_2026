#ifndef ESTADOS_H
#define ESTADOS_H

#include "sistema.h"

estado_t f_espera(sistema_t *sistema);
estado_t f_llenado(sistema_t *sistema);
estado_t f_alarma(sistema_t *sistema);
estado_t f_recuperacion(sistema_t *sistema);
estado_t f_vaciando(sistema_t *sistema);
estado_t f_inicio(sistema_t *sistema);

#endif
