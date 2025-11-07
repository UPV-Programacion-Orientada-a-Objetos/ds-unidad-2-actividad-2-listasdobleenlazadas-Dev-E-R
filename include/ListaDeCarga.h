/**
 * @file ListaDeCarga.h
 * @brief Lista doblemente enlazada para almacenar caracteres decodificados
 * @author Equipo de Desarrollo
 * @version 1.0
 * @date 2024
 */

#ifndef LISTADECARGA_H
#define LISTADECARGA_H

#include <iostream>

/**
 * @class ListaDeCarga
 * @brief Lista doblemente enlazada implementada manualmente para almacenar el mensaje decodificado
 * 
 * Esta clase implementa una lista doblemente enlazada desde cero (sin STL)
 * que mantiene el orden de los caracteres decodificados del protocolo PRT-7.
 * 
 * @note Prohibido el uso de std::list o cualquier contenedor STL
 */
class ListaDeCarga
{
private:
    /* data */
    
public:
    /**
     * @struct Nodo
     * @brief Nodo individual de la lista doblemente enlazada
     * 
     * Cada nodo contiene un carácter y punteros al nodo anterior y siguiente.
     */
    struct Nodo
    {
        char dato;      ///< Carácter almacenado en este nodo
        Nodo* sig;      ///< Puntero al siguiente nodo
        Nodo* ant;      ///< Puntero al nodo anterior
        
        /**
         * @brief Constructor del nodo
         * @param contenido Carácter a almacenar en el nodo
         */
        Nodo(char contenido) : dato(contenido), sig(nullptr), ant(nullptr) {}
    };

    Nodo* cabeza;  ///< Puntero al primer nodo de la lista
    Nodo* cola;    ///< Puntero al último nodo de la lista

    /**
     * @brief Inserta un carácter al final de la lista
     * @param dato Carácter a insertar
     * 
     * Complejidad: O(1) gracias al puntero cola
     */
    void insertarAlFinal(char dato){
        Nodo* nuevo = new Nodo(dato);
                
        if (cabeza == nullptr) {
            cabeza = nuevo;
            cola = nuevo;
        } else {
            cola->sig = nuevo;
            nuevo->ant = cola;
            cola = nuevo;
        }
    }

    /**
     * @brief Imprime el mensaje completo de forma recursiva
     * @param Lista Nodo desde el cual comenzar la impresión
     * 
     * Recorre la lista de forma recursiva e imprime cada carácter
     * sin espacios ni saltos de línea intermedios.
     */
    void imprimirMensaje(Nodo* Lista){
        if (Lista == nullptr) {
            std::cout << "nullptr" << std::endl;
            return;
        }
        std::cout << Lista->dato;
        if (Lista == cola)
        {
            return;
        }
        
        imprimirMensaje(Lista->sig);
    }

    /**
     * @brief Constructor por defecto
     * 
     * Inicializa la lista vacía con cabeza y cola en nullptr.
     */
    ListaDeCarga(){
        cabeza = nullptr;
        cola = nullptr;
    }
    
    /**
     * @brief Destructor
     * 
     * Libera toda la memoria dinámica utilizada por los nodos de la lista.
     * Recorre la lista y elimina cada nodo para evitar fugas de memoria.
     */
    ~ListaDeCarga() {
        Nodo* actual = cabeza;
        while (actual != nullptr) {
            Nodo* siguiente = actual->sig;
            delete actual;
            actual = siguiente;
        }
        std::cout << "[ListaDeCarga] Destruida.... Sistema Apagado" << std::endl;
    }
};

#endif
