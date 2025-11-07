/**
 * @file TramaBase.h
 * @brief Clase base abstracta para tramas del protocolo PRT-7
 * @author Equipo de Desarrollo
 * @version 1.0
 * @date 2024
 */

#ifndef TRAMABASE_H
#define TRAMABASE_H

#include <iostream>

class ListaDeCarga;
class RotorDeMapeo;

/**
 * @class TramaBase
 * @brief Clase base abstracta que define la interfaz para todas las tramas del protocolo PRT-7
 * 
 * Esta clase utiliza polimorfismo para permitir el procesamiento uniforme
 * de diferentes tipos de tramas (LOAD y MAP) recibidas del puerto serial.
 * 
 * @note Esta es una clase abstracta que no puede ser instanciada directamente.
 * @warning El destructor DEBE ser virtual para evitar fugas de memoria al usar polimorfismo.
 */
class TramaBase
{
private:
    /* data */

public:
    /**
     * @brief Constructor por defecto
     */
    TramaBase() {
        // Constructor vacío
    }

    /**
     * @brief Procesa la trama según su tipo específico (método virtual puro)
     * @param carga Puntero a la lista donde se almacenan los caracteres decodificados
     * @param rotor Puntero al rotor de mapeo que realiza la decodificación
     * 
     * Este método debe ser implementado por todas las clases derivadas.
     * Define el comportamiento específico de cada tipo de trama.
     */
    virtual void procesar(ListaDeCarga* carga, RotorDeMapeo* rotor) = 0;

    /**
     * @brief Destructor virtual
     * 
     * CRÍTICO: El destructor debe ser virtual para garantizar la correcta
     * liberación de memoria cuando se eliminan objetos derivados a través
     * de punteros de la clase base.
     */
    virtual ~TramaBase() {
    }
};

#endif
