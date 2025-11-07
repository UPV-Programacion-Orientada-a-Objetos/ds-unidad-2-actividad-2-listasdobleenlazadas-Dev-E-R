/**
 * @file TramaLoad.h
 * @brief Clase que representa una trama de tipo LOAD (carga de datos)
 * @author Equipo de Desarrollo
 * @version 1.0
 * @date 2024
 */

#ifndef TRAMALOAD_H
#define TRAMALOAD_H

#include "TramaBase.h"
#include "ListaDeCarga.h"
#include "RotorDeMapeo.h"

/**
 * @class TramaLoad
 * @brief Trama que contiene un carácter a decodificar
 * 
 * Las tramas LOAD representan fragmentos de datos (L,X) donde X es un carácter.
 * Al procesarse, el carácter se mapea usando el estado actual del rotor
 * y se agrega a la lista de carga.
 * 
 * Ejemplo de trama serial: "L,H" -> letra='H'
 */
class TramaLoad : public TramaBase
{
private:
    char letra;  ///< Carácter a decodificar
    
public:
    /**
     * @brief Procesa la trama LOAD
     * @param carga Puntero a la lista donde se almacenará el carácter decodificado
     * @param rotor Puntero al rotor usado para el mapeo
     * 
     * Funcionamiento:
     * 1. Obtiene el carácter mapeado usando rotor->getMapeo(letra)
     * 2. Inserta el carácter decodificado en la lista de carga
     */
    void procesar(ListaDeCarga* carga, RotorDeMapeo* rotor) override {
        char decodificado = rotor->getMapeo(letra);
        carga->insertarAlFinal(decodificado);
    }

    /**
     * @brief Constructor
     * @param l Carácter que contiene la trama
     */
    TramaLoad(char l) : letra(l) {
    }
    
    /**
     * @brief Destructor
     */
    ~TramaLoad() {
    }
};

#endif
