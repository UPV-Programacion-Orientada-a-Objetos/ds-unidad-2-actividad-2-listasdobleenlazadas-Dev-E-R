/**
 * @file main.cpp
 * @brief Programa principal del Decodificador PRT-7
 * @author Equipo de Desarrollo
 * @version 1.0
 * @date 2024
 * 
 * Este programa implementa un decodificador para el protocolo industrial PRT-7
 * que recibe tramas desde un dispositivo Arduino vía puerto serial.
 */

#include <iostream>
#include <cstring>
#include "TramaBase.h"
#include "ListaDeCarga.h"
#include "RotorDeMapeo.h"
#include "SerialPort.h"
#include "TramaLoad.h"
#include "TramaMap.h"

/**
 * @brief Procesa una línea recibida del puerto serial
 * @param lineas Cadena con formato "X,Y" donde X es el comando (L/M) e Y es el dato
 * 
 * Esta función parsea la trama, crea el objeto polimórfico correspondiente
 * (TramaLoad o TramaMap), lo procesa y libera su memoria.
 */
void cargarLineas(char*);

// Estructuras de datos globales
ListaDeCarga ListaCarga;  ///< Lista que almacena el mensaje decodificado
RotorDeMapeo RotorMapeo;  ///< Rotor circular para el mapeo de caracteres

/**
 * @brief Función principal del programa
 * @return 0 si la ejecución fue exitosa
 * 
 * Flujo del programa:
 * 1. Inicializa las estructuras de datos (ListaCarga vacía, RotorMapeo con A-Z)
 * 2. Abre el puerto serial y reinicia el dispositivo Arduino
 * 3. Lee las tramas del puerto y las procesa una por una
 * 4. Imprime el mensaje decodificado final
 * 5. Libera memoria y cierra recursos
 */
int main()
{
    // Crear instancia del puerto serial
    SerialPort puerto;
    std::cout << "=== Decodificador  PRT-7 ===" << std::endl << std::endl;
    
    if (puerto.abrir("/dev/ttyUSB0", 9600)) {
        std::cout << "Esperando datos de Arduino..." << std::endl;
        
        // Reiniciar el ESP32 para que envíe datos desde el inicio
        puerto.reiniciarDispositivo();
        
        char lineas[100];
        
        // Leer tramas (ciclo completo del sketch)
        for (int i = 0; i < 14; i++) {
            if (puerto.leerLinea(lineas)) {
                cargarLineas(lineas);
            }
        }
        puerto.cerrar();
    }

    // Mostrar el mensaje decodificado
    std::cout << " --- " << std::endl;
    std::cout << " MENSAJE OCULTO ENSAMBLADO " << std::endl;
    ListaCarga.imprimirMensaje(ListaCarga.cabeza);
    std::cout << std::endl;
    std::cout << " --- " << std::endl;

    return 0;
}

/**
 * @brief Procesa y parsea una línea recibida del puerto serial
 * @param lineas Buffer que contiene la trama recibida (formato: "X,Y")
 * 
 * Funcionamiento:
 * 1. Valida que la trama tenga formato correcto
 * 2. Separa el comando (L/M) del dato
 * 3. Crea dinámicamente el objeto Trama correspondiente (polimorfismo)
 * 4. Ejecuta el procesamiento llamando a trama->procesar()
 * 5. Libera la memoria del objeto creado
 * 
 * @note Usa gestión manual de memoria (new/delete) sin STL
 */
void cargarLineas(char* lineas){
    if (lineas == nullptr || lineas[0] == '\0') {
        std::cout << "[ADVERTENCIA] Trama vacia recibida" << std::endl;
        return;
    }

    std::cout << "Trama recibida: [" << lineas << "] -> Procesando...." << std::endl; 
    
    // Buscar el separador ','
    int posicionComa = -1;
    int longitud = 0;
    
    for (int i = 0; lineas[i] != '\0'; i++) {
        if (lineas[i] == ',') {
            posicionComa = i;
        }
        longitud++;
    }
    
    // Validar formato de trama
    if (posicionComa == -1 || posicionComa == 0 || posicionComa == longitud - 1) {
        std::cout << "[ERROR] Formato de trama invalido: " << lineas << std::endl;
        return;
    }
    
    // Extraer comando y dato
    char comando = lineas[0];
    const char* dato = &lineas[posicionComa + 1];
    
    // Interpretar según el tipo de comando (polimorfismo)
    switch(comando) {
        case 'L':
        case 'l':
            {
                // Trama LOAD: cargar un carácter
                char letra;
                if (strcmp(dato, "Space") == 0 || strcmp(dato, "space") == 0) {
                    letra = ' ';
                } else {
                    letra = dato[0];
                }
                
                TramaBase* trama = new TramaLoad(letra);
                trama->procesar(&ListaCarga, &RotorMapeo);
                delete trama;
            }
            break;
            
        case 'M':
        case 'm':
            {
                // Trama MAP: rotar el rotor
                int movimiento = atoi(dato);
                TramaBase* trama = new TramaMap(movimiento);
                trama->procesar(&ListaCarga, &RotorMapeo);
                delete trama;
            }
            break;
            
        default:
            std::cout << "[DESCONOCIDO] Comando: " << comando << ", Dato: " << dato << std::endl;
            break;
    }
}
