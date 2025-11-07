/**
 * @file SerialPort.h
 * @brief Clase para comunicacion serial con dispositivos externos
 * @author Sistema de Monitoreo
 * @version 1.0
 * @date 2024
 */

#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <string>
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <sys/ioctl.h>

/**
 * @class SerialPort
 * @brief Clase para comunicacion con dispositivos via puerto serial
 * 
 * Proporciona funcionalidad para establecer conexion, configurar parametros
 * y leer datos desde dispositivos conectados via puerto serial.
 * Compatible con sistemas Linux y Mac.
 */
class SerialPort {
private:
    int descriptorArchivo;  ///< Descriptor del archivo de puerto
    bool estadoConexion;    ///< Estado actual de la conexion
    
public:
    /**
     * @brief Constructor por defecto
     * 
     * Inicializa el puerto serial en estado cerrado.
     */
    SerialPort() : descriptorArchivo(-1), estadoConexion(false) {}
    
    /**
     * @brief Establece conexion con puerto serial
     * @param rutaPuerto Direccion del dispositivo (ejemplo: /dev/ttyACM0)
     * @param velocidad Tasa de transmision en baudios (predeterminado 9600)
     * @return true si la conexion fue exitosa, false en caso contrario
     * 
     * Configura el puerto serial con los parametros especificados:
     * - Velocidad de transmision (9600, 19200, 38400, 57600, 115200 bps)
     * - Formato 8N1 (8 bits de datos, sin paridad, 1 bit de parada)
     * - Modo sin procesar (raw mode)
     */
    bool abrir(const std::string& rutaPuerto, int velocidad = 9600) {
        descriptorArchivo = open(rutaPuerto.c_str(), O_RDONLY | O_NOCTTY);
        
        if (descriptorArchivo < 0) {
            std::cerr << "[ERROR] Imposible establecer conexion con " << rutaPuerto << std::endl;
            std::cerr << "Verificaciones requeridas:" << std::endl;
            std::cerr << "  1. Confirmar conexion fisica del dispositivo" << std::endl;
            std::cerr << "  2. Otorgar permisos necesarios (sudo chmod 666 " << rutaPuerto << ")" << std::endl;
            return false;
        }
        
        // Establecer parametros de comunicacion
        struct termios configuracion;
        tcgetattr(descriptorArchivo, &configuracion);
        
        // Asignar velocidad de transmision
        speed_t tasaBaudios;
        switch(velocidad) {
            case 9600:  tasaBaudios = B9600;  break;
            case 19200: tasaBaudios = B19200; break;
            case 38400: tasaBaudios = B38400; break;
            case 57600: tasaBaudios = B57600; break;
            case 115200: tasaBaudios = B115200; break;
            default:    tasaBaudios = B9600;  break;
        }
        
        cfsetispeed(&configuracion, tasaBaudios);
        cfsetospeed(&configuracion, tasaBaudios);
        
        // Parametros de protocolo: 8N1
        configuracion.c_cflag &= ~PARENB;  // Desactivar bit de paridad
        configuracion.c_cflag &= ~CSTOPB;  // Un bit de detencion
        configuracion.c_cflag &= ~CSIZE;
        configuracion.c_cflag |= CS8;      // Ocho bits de informacion
        
        configuracion.c_cflag |= (CLOCAL | CREAD);  // Activar recepcion
        
        // Configuracion en modo sin procesar
        configuracion.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        configuracion.c_iflag &= ~(IXON | IXOFF | IXANY);
        configuracion.c_oflag &= ~OPOST;
        
        // Implementar parametros
        tcsetattr(descriptorArchivo, TCSANOW, &configuracion);
        
        // Limpiar buffers de entrada/salida
        tcflush(descriptorArchivo, TCIOFLUSH);
        
        estadoConexion = true;
        std::cout << "[OK] Puerto " << rutaPuerto << " habilitado a " 
                  << velocidad << " baudios" << std::endl;
        
        // Periodo de estabilizacion del dispositivo
        sleep(2);
        
        return true;
    }
    
    /**
     * @brief Captura una secuencia de caracteres del puerto
     * @param secuencia Contenedor para almacenar la cadena recibida (buffer)
     * @param maxLength Tamaño máximo del buffer (por defecto 100)
     * @return true si la lectura fue exitosa, false en caso contrario
     * 
     * Lee caracteres del puerto serial hasta encontrar un terminador de linea
     * (nueva linea o retorno de carro). Espera activamente hasta recibir datos.
     * Compatible con formato Arduino Serial.println()
     */
    bool leerLinea(char* secuencia, int maxLength = 100) {
        if (!estadoConexion || descriptorArchivo < 0) {
            return false;
        }
        
        int indice = 0;
        secuencia[0] = '\0';
        char caracter;
        ssize_t bytesCapturados;
        
        while (true) {
            bytesCapturados = read(descriptorArchivo, &caracter, 1);
            
            if (bytesCapturados < 0) {
                std::cerr << "[ERROR] Fallo en lectura de puerto serial" << std::endl;
                return false;
            }
            
            if (bytesCapturados == 0) {
                usleep(10000);
                continue;
            }
            
            if (caracter == '\n' || caracter == '\r') {
                if (indice > 0) {
                    secuencia[indice] = '\0';
                    return true;
                }
                continue;
            }
            
            if (indice < maxLength - 1) {
                secuencia[indice++] = caracter;
            }
        }
    }
    
    /**
     * @brief Consulta estado de la conexion
     * @return true si el puerto esta abierto, false en caso contrario
     */
    bool estaAbierto() const {
        return estadoConexion;
    }
    
    /**
     * @brief Reinicia el ESP32/Arduino mediante DTR
     */
    void reiniciarDispositivo() {
        if (!estadoConexion || descriptorArchivo < 0) {
            std::cerr << "[ERROR] No hay conexion activa para reiniciar" << std::endl;
            return;
        }
        
        std::cout << "[INFO] Reiniciando dispositivo..." << std::endl;
        
        int estado = TIOCM_DTR;
        ioctl(descriptorArchivo, TIOCMBIC, &estado);
        usleep(100000);
        
        ioctl(descriptorArchivo, TIOCMBIS, &estado);
        usleep(100000);
        
        tcflush(descriptorArchivo, TCIOFLUSH);
        
        std::cout << "[OK] Dispositivo reiniciado. Esperando estabilizacion..." << std::endl;
        sleep(2);
    }
    
    /**
     * @brief Finaliza la comunicacion serial
     */
    void cerrar() {
        if (estadoConexion && descriptorArchivo >= 0) {
            close(descriptorArchivo);
            descriptorArchivo = -1;
            estadoConexion = false;
            std::cout << "Conexion serial finalizada" << std::endl;
        }
    }
    
    /**
     * @brief Destructor de la clase
     */
    ~SerialPort() {
        cerrar();
    }
};

#endif
