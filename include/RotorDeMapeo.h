/**
 * @file RotorDeMapeo.h
 * @brief Lista circular doblemente enlazada que actúa como disco de cifrado
 * @author Equipo de Desarrollo
 * @version 1.0
 * @date 2024
 */

#ifndef ROTORDEMAPEO_H
#define ROTORDEMAPEO_H

#include <iostream>

/**
 * @class RotorDeMapeo
 * @brief Lista circular doblemente enlazada que implementa el mecanismo de cifrado tipo César
 * 
 * Esta clase implementa una rueda de cifrado que contiene el alfabeto A-Z.
 * La rotación del puntero 'cabeza' cambia el mapeo de cada letra, similar
 * a las máquinas de cifrado históricas como Enigma.
 * 
 * @note Implementación manual sin uso de STL
 */
class RotorDeMapeo
{
private:
    /* data */
    
public:
    /**
     * @struct NodoMap
     * @brief Nodo de la lista circular que contiene una letra del alfabeto
     */
    struct NodoMap
    {
        char dato;      ///< Letra del alfabeto (A-Z)
        NodoMap* sig;   ///< Puntero al siguiente nodo (circular)
        NodoMap* ant;   ///< Puntero al nodo anterior (circular)

        /**
         * @brief Constructor del nodo
         * @param contenido Letra a almacenar
         */
        NodoMap(char contenido) : dato(contenido), sig(nullptr), ant(nullptr) {}
    };
    
    NodoMap* cabeza = nullptr;  ///< Puntero a la posición 'cero' del rotor
    NodoMap* cola = nullptr;    ///< Puntero al último nodo insertado

    /**
     * @brief Constructor que inicializa el rotor con el alfabeto A-Z
     * 
     * Crea una lista circular con las 26 letras del alfabeto.
     * Inicialmente, cabeza apunta a 'A' (posición 0).
     */
    RotorDeMapeo(){
        const char alfabeto[26] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

        for(char letra: alfabeto){
            insertarLetra(letra);
        }
    }

    /**
     * @brief Obtiene el carácter mapeado según la rotación actual del rotor
     * @param letra Carácter de entrada a mapear
     * @return Carácter resultante después del mapeo
     * 
     * Lógica de mapeo (tipo César):
     * 1. Calcula la posición de 'letra' en el alfabeto original (A=0, B=1, ..., Z=25)
     * 2. Avanza esa cantidad de posiciones desde la 'cabeza' actual del rotor
     * 3. Retorna el carácter encontrado en esa posición
     * 
     * Ejemplo:
     * - Si cabeza apunta a 'A' y letra='C', retorna 'C'
     * - Si cabeza apunta a 'C' (rotado +2) y letra='A', retorna 'C'
     * - Si cabeza apunta a 'C' (rotado +2) y letra='W', retorna 'Y'
     * 
     * @note Los caracteres fuera del rango A-Z se retornan sin cambios
     */
    char getMapeo(char letra){
        if (cabeza == nullptr) {
            return letra;
        }
        
        // Manejar espacios y caracteres especiales
        if (letra == ' ') {
            return ' ';
        }
        
        // Calcular la posición de la letra en el alfabeto original (A=0, B=1, etc.)
        int posicionOriginal = letra - 'A';
        
        // Si la letra no está en rango A-Z, retornarla sin cambios
        if (posicionOriginal < 0 || posicionOriginal >= 26) {
            return letra;
        }
        
        // Avanzar desde la cabeza actual (que ya está rotada)
        // la cantidad de posiciones correspondiente a la letra original
        NodoMap* resultado = cabeza;
        for (int i = 0; i < posicionOriginal; i++) {
            resultado = resultado->sig;
        }
        
        return resultado->dato;
    }

    /**
     * @brief Rota el rotor N posiciones
     * @param n Número de posiciones a rotar
     *          - Positivo: rota hacia adelante (A->B->C...)
     *          - Negativo: rota hacia atrás (A->Z->Y...)
     *          - Cero: no hace nada
     * 
     * La rotación solo mueve el puntero 'cabeza', no los datos.
     * Esto hace que el mapeo de cada letra cambie dinámicamente.
     * 
     * Complejidad: O(|n|) donde |n| es el valor absoluto de n
     */
    void rotar(int n){
        if (cabeza == nullptr || n == 0) {
            return;
        }
        
        if (n > 0) {
            // Rotar hacia adelante (siguiente)
            for (int i = 0; i < n; i++) {
                cabeza = cabeza->sig;
            }
        } else {
            // Rotar hacia atrás (anterior)
            for (int i = 0; i < -n; i++) {
                cabeza = cabeza->ant;
            }
        }
    }

    /**
     * @brief Inserta una letra en la lista circular
     * @param letra Letra a insertar
     * 
     * Método auxiliar usado durante la construcción del rotor.
     * Mantiene las propiedades de lista circular doblemente enlazada.
     */
    void insertarLetra(char letra){
        NodoMap *nuevo_nodo = new NodoMap(letra);
        
        // Si la lista está vacía
        if (!cabeza)
        {
            cabeza = nuevo_nodo;
            cola  = nuevo_nodo;
            // Para lista circular, apunta a sí mismo
            nuevo_nodo->sig = nuevo_nodo;
            nuevo_nodo->ant = nuevo_nodo;
            return;
        }
        
        // Insertar al final (después de cola)
        nuevo_nodo->ant = cola;
        nuevo_nodo->sig = cabeza;
        cola->sig = nuevo_nodo;
        cabeza->ant = nuevo_nodo;
        cola = nuevo_nodo;
    }

    /**
     * @brief Muestra la lista circular de forma recursiva (para debugging)
     * @param Lista Nodo desde donde comenzar
     */
    void mostrarLista(NodoMap* Lista) {
        if (Lista == nullptr) {
            std::cout << "nullptr" << std::endl;
            return;
        }
        std::cout << Lista->dato << " - ";
        if (Lista == cola)
        {
            std::cout << "Final del circulo" << std::endl;
            return;
        }
        
        mostrarLista(Lista->sig);
    }
    
    /**
     * @brief Destructor
     * 
     * Libera la memoria de todos los nodos de la lista circular.
     * Debe romper el círculo antes de eliminar para evitar bucles infinitos.
     */
    ~RotorDeMapeo(){
        if (cabeza == nullptr) return;
        
        // Romper el círculo
        cola->sig = nullptr;
        
        // Eliminar todos los nodos
        NodoMap* actual = cabeza;
        while (actual != nullptr) {
            NodoMap* siguiente = actual->sig;
            delete actual;
            actual = siguiente;
        }
    }
};

#endif
