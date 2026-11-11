#include <stddef.h>
#include "estados.h"

static bool condicion_alarma(const sistema_t *sistema)
{
    return (*sistema).falla_activa ||
           (*sistema).paro_emergencia ||
           (*sistema).nivel_l >= (CAPACIDAD_TANQUE_L * NIVEL_CRITICO / 100.0f);
}

static void entrar_en_alarma(sistema_t *sistema, estado_t estado_actual)
{
    if (estado_actual != ALARMA) 
    {
        (*sistema).estado_anterior = estado_actual;
        (*sistema).estado_anterior_valido = true;
    }

    (*sistema).bomba_encendida = false;
    (*sistema).valvula_vaciado_abierta = false;
    (*sistema).alarma_activa = true;
}

estado_t f_inicio(sistema_t *sistema)
{
    if (sistema == NULL) return ALARMA;

    (*sistema).bomba_encendida = false;
    (*sistema).valvula_vaciado_abierta = false;
    (*sistema).alarma_activa = false;

    if (condicion_alarma(sistema)) 
    {
        (*sistema).alarma_activa = true;
        return ALARMA;
    }

    if ((*sistema).nivel_l <= (CAPACIDAD_TANQUE_L * NIVEL_INICIO_LLENADO / 100.0f)) 
    {
        return LLENADO;
    }

    return ESPERA;
}

estado_t f_espera(sistema_t *sistema)
{
    if (sistema == NULL) return ALARMA;

    (*sistema).bomba_encendida = false;
    (*sistema).valvula_vaciado_abierta = false;

    if (condicion_alarma(sistema)) 
    {
        entrar_en_alarma(sistema, ESPERA);
        return ALARMA;
    }

    /* En ESPERA solo se inicia llenado cuando el nivel desciende al 25% o menos */
    if ((*sistema).nivel_l <= (CAPACIDAD_TANQUE_L * NIVEL_INICIO_LLENADO / 100.0f)) 
    {
        return LLENADO;
    }

    return ESPERA;
}

estado_t f_llenado(sistema_t *sistema)
{
    if (sistema == NULL) return ALARMA;

    if (condicion_alarma(sistema)) 
    {
        entrar_en_alarma(sistema, LLENADO);
        return ALARMA;
    }

    (*sistema).valvula_vaciado_abierta = false;
    (*sistema).bomba_encendida = true;

    /* Alcanzado el 90%, se apaga bomba y vuelve a espera */
    if ((*sistema).nivel_l >= (CAPACIDAD_TANQUE_L * NIVEL_FIN_LLENADO / 100.0f)) 
    {
        (*sistema).bomba_encendida = false;
        return ESPERA;
    }

    return LLENADO;
}

estado_t f_alarma(sistema_t *sistema)
{
    if (sistema == NULL) return ALARMA;

    (*sistema).bomba_encendida = false;
    (*sistema).valvula_vaciado_abierta = false;
    (*sistema).alarma_activa = true;

    if ((*sistema).reset_solicitado &&
        !(*sistema).falla_activa &&
        !(*sistema).paro_emergencia &&
        (*sistema).nivel_l < (CAPACIDAD_TANQUE_L * NIVEL_CRITICO / 100.0f)) 
    {

        (*sistema).reset_solicitado = false;
        (*sistema).alarma_activa = false;
        return RECUPERACION;
    }

    (*sistema).reset_solicitado = false;
    return ALARMA;
}

estado_t f_recuperacion(sistema_t *sistema)
{
    if (sistema == NULL) 
    return ALARMA;

    (*sistema).bomba_encendida = false;
    (*sistema).valvula_vaciado_abierta = false;
    (*sistema).alarma_activa = false;

    if (condicion_alarma(sistema)) 
    {
        entrar_en_alarma(sistema, RECUPERACION);
        return ALARMA;
    }

    switch ((*sistema).accion_recuperacion) 
    {
        case RECUPERAR_VACIAR:
            (*sistema).estado_anterior_valido = false;
            return VACIANDO;

        case RECUPERAR_REINICIAR:
            (*sistema).estado_anterior_valido = false;
            return ESPERA;

        case RECUPERAR_REANUDAR:
        default:
            if ((*sistema).estado_anterior_valido) 
            {
                estado_t estado_guardado = (*sistema).estado_anterior;
                (*sistema).estado_anterior_valido = false;

                if (estado_guardado == LLENADO &&(*sistema).nivel_l < (CAPACIDAD_TANQUE_L * NIVEL_FIN_LLENADO / 100.0f)) 
                {
                    return LLENADO;
                }

                return ESPERA;
            }

            return ESPERA;
    }
}

estado_t f_vaciando(sistema_t *sistema)
{
    if (sistema == NULL) return ALARMA;

    if (condicion_alarma(sistema)) 
    {
        entrar_en_alarma(sistema, VACIANDO);
        return ALARMA;
    }

    (*sistema).bomba_encendida = false;
    (*sistema).valvula_vaciado_abierta = true;

    if ((*sistema).nivel_l <= 0.0f) 
    {
        (*sistema).valvula_vaciado_abierta = false;
        return ESPERA;
    }

    return VACIANDO;
}
