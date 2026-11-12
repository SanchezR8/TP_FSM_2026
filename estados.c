#include "mylib.h"

static estados_t estado_anterior = ESPERA;
static unsigned char estado_anterior_valido = 0;

static unsigned char condicion_alarma(void) {
    if (driver_falla_activa() ||
        driver_paro_emergencia() ||
        driver_leer_nivel_l() >= NIVEL_CRITICO) {
        return 1;
    }
    return 0;
}

static estados_t ir_a_alarma(estados_t estado_actual) {
    if (estado_actual != ALARMA) {
        estado_anterior = estado_actual;
        estado_anterior_valido = 1;
    }
    driver_bomba_off();
    driver_valvula_off();
    driver_alarma_on();
    return ALARMA;
}

estados_t f_inicio(void) {
    driver_bomba_off();
    driver_valvula_off();
    driver_alarma_off();

    if (condicion_alarma()) {
        return ir_a_alarma(INICIO);
    }

    if (driver_leer_nivel_l() <= NIVEL_INICIO_LLENADO) {
        return LLENADO;
    }

    return ESPERA;
}

estados_t f_espera(void) {
    driver_bomba_off();
    driver_valvula_off();

    if (condicion_alarma()) {
        return ir_a_alarma(ESPERA);
    }

    if (driver_leer_nivel_l() <= NIVEL_INICIO_LLENADO) {
        return LLENADO;
    }

    return ESPERA;
}

estados_t f_llenado(void) {
    if (condicion_alarma()) {
        return ir_a_alarma(LLENADO);
    }

    driver_valvula_off();
    driver_bomba_on();

    if (driver_leer_nivel_l() >= NIVEL_FIN_LLENADO) {
        driver_bomba_off();
        return ESPERA;
    }

    return LLENADO;
}

estados_t f_alarma(void) {
    driver_bomba_off();
    driver_valvula_off();
    driver_alarma_on();

    if (driver_reset_solicitado() && !condicion_alarma()) {
        driver_alarma_off();
        return RECUPERACION;
    }

    return ALARMA;
}

estados_t f_recuperacion(void) {
    driver_bomba_off();
    driver_valvula_off();
    driver_alarma_off();

    if (condicion_alarma()) {
        return ir_a_alarma(RECUPERACION);
    }

    accion_recuperacion_t accion = driver_modo_recuperacion();

    if (accion == RECUPERAR_VACIAR) {
        estado_anterior_valido = 0;
        return VACIANDO;
    }

    if (accion == RECUPERAR_REINICIAR) {
        estado_anterior_valido = 0;
        return ESPERA;
    }

    if (estado_anterior_valido) {
        estados_t guardado = estado_anterior;
        estado_anterior_valido = 0;

        if (guardado == LLENADO && driver_leer_nivel_l() < NIVEL_FIN_LLENADO) {
            return LLENADO;
        }
    }

    return ESPERA;
}

estados_t f_vaciando(void) {
    if (condicion_alarma()) {
        return ir_a_alarma(VACIANDO);
    }

    driver_bomba_off();
    driver_valvula_on();

    if (driver_leer_nivel_l() == 0) {
        driver_valvula_off();
        return ESPERA;
    }

    return VACIANDO;
}
