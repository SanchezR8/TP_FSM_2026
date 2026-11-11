#include <stddef.h>
#include "sistema.h"

const char *estado_nombre(estado_t estado)
{
    switch (estado) 
    {
        case ESPERA:       return "ESPERA";
        case LLENADO:      return "LLENADO";
        case ALARMA:       return "ALARMA";
        case RECUPERACION: return "RECUPERACION";
        case VACIANDO:     return "VACIANDO";
        default:           return "DESCONOCIDO";
    }
}

void sistema_inicializar(sistema_t *sistema, float nivel_inicial_l)
{
    if (sistema == NULL) return;

    if (nivel_inicial_l < 0.0f) nivel_inicial_l = 0.0f;
    if (nivel_inicial_l > CAPACIDAD_TANQUE_L) nivel_inicial_l = CAPACIDAD_TANQUE_L;

    (*sistema).nivel_l = nivel_inicial_l;
    (*sistema).bomba_encendida = false;
    (*sistema).valvula_vaciado_abierta = false;
    (*sistema).alarma_activa = false;
    (*sistema).falla_activa = false;
    (*sistema).paro_emergencia = false;
    (*sistema).reset_solicitado = false;
    (*sistema).accion_recuperacion = RECUPERAR_REANUDAR;
    (*sistema).estado_anterior = ESPERA;
    (*sistema).estado_anterior_valido = false;
}

void sistema_aplicar_seguridad(sistema_t *sistema)
{
    if (sistema == NULL) return;

    /* Enclavamiento por software: nunca bomba y valvula simultaneas */
    if ((*sistema).bomba_encendida && (*sistema).valvula_vaciado_abierta) 
    {
        (*sistema).bomba_encendida = false;
    }

    /* Parada total ante cualquier condicion insegura */
    if ((*sistema).falla_activa || (*sistema).paro_emergencia || (*sistema).nivel_l >= (CAPACIDAD_TANQUE_L * NIVEL_CRITICO / 100.0f)) 
    {
        (*sistema).bomba_encendida = false;
        (*sistema).valvula_vaciado_abierta = false;
    }
}
