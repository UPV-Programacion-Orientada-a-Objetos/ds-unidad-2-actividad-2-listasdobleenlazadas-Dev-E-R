/**
 * @file TramaMap.h
 * @brief Clase que representa una trama de tipo MAP (modificación del rotor)
 * @author Equipo de Desarrollo
 * @version 1.0
 * @date 2024
 */

#ifndef TRAMAMAP_H
#define TRAMAMAP_H

#include "TramaBase.h"
#include "RotorDeMapeo.h"

/**
 * @class TramaMap
 * @brief Trama que modifica la rotación del rotor de mapeo
 * 
 * Las tramas MAP representan instrucciones de rotación (M,N) donde N es un entero.
 * Al procesarse, el rotor gira N posiciones, cambiando el mapeo de todas
 * las tramas LOAD subsecuentes.
 * 
 * Ejemplos de tramas seriales:
 * - "M,2"  -> rotar +2 posiciones (A->C)
 * - "M,-2" -> rotar -2 posiciones (A->Y)
 */
class TramaMap : public TramaBase
{
private:
    int rotacion;  ///< Cantidad de posiciones a rotar
    
public:
    /**
     * @brief Procesa la trama MAP
     * @param carga Puntero a la lista de carga (no utilizado por MAP)
     * @param rotor Puntero al rotor que será rotado
     * 
     * Simplemente llama a rotor->rotar(rotacion) para modificar
     * el estado del disco de cifrado.
     */
    void procesar(ListaDeCarga* carga, RotorDeMapeo* rotor) override {
        rotor->rotar(rotacion);
    }

    /**
     * @brief Constructor
     * @param rota Número de posiciones a rotar (puede ser positivo o negativo)
     */
    TramaMap(int rota) : rotacion(rota) {
    }
    
    /**
     * @brief Destructor
     */
    ~TramaMap() {
    }
};

#endif
