#ifndef SISTEMA_H
#define SISTEMA_H

#include <stdbool.h>
#include <stdint.h>

/* Parametros fiisicos del proceso */
#define CAPACIDAD_TANQUE_L       1000.0f
#define NIVEL_INICIO_LLENADO       25.0f   // % minimo para disparar llenado 
#define NIVEL_FIN_LLENADO          90.0f   // % meta de llenado 
#define NIVEL_CRITICO              95.0f   // % de sobrellenado (alarma critica) 

typedef enum 
{
    ESPERA = 0,
    LLENADO,
    ALARMA,
    RECUPERACION,
    VACIANDO,
    CANTIDAD_ESTADOS
} estado_t;

typedef enum 
{
    RECUPERAR_REANUDAR = 1,
    RECUPERAR_REINICIAR,
    RECUPERAR_VACIAR
} accion_recuperacion_t;

typedef struct 
{
    float nivel_l;
    bool bomba_encendida;
    bool valvula_vaciado_abierta;
    bool alarma_activa;

    bool falla_activa;
    bool paro_emergencia;

    bool reset_solicitado;
    accion_recuperacion_t accion_recuperacion;

    estado_t estado_anterior;
    bool estado_anterior_valido;
} sistema_t;

const char *estado_nombre(estado_t estado);
void sistema_inicializar(sistema_t *sistema, float nivel_inicial_l);
void sistema_aplicar_seguridad(sistema_t *sistema);

#endif
