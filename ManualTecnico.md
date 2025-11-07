# Manual Técnico - Decodificador PRT-7

**Versión:** 1.0  
**Fecha:** 6 de Noviembre de 2025  
**Autor:** Dev-E-R  
**Asignatura:** Programación Orientada a Objetos  
**Institución:** Universidad Politécnica de Valencia

---

## Tabla de Contenidos

1. [Introducción](#introducción)
2. [Arquitectura del Sistema](#arquitectura-del-sistema)
3. [Estructuras de Datos](#estructuras-de-datos)
4. [Módulos del Sistema](#módulos-del-sistema)
5. [Protocolo de Comunicación](#protocolo-de-comunicación)
6. [Compilación e Instalación](#compilación-e-instalación)
7. [Configuración](#configuración)
8. [Diagrama de Flujo](#diagrama-de-flujo)
9. [Gestión de Memoria](#gestión-de-memoria)
10. [Resolución de Problemas](#resolución-de-problemas)
11. [Especificaciones Técnicas](#especificaciones-técnicas)

---

## Introducción

### Descripción General

El Decodificador PRT-7 es un sistema de software desarrollado en C++ para la decodificación de mensajes encriptados transmitidos desde dispositivos Arduino/ESP32 mediante comunicación serial. El sistema implementa un algoritmo de rotación de caracteres basado en el principio del cifrado César mediante estructuras de datos manuales (listas enlazadas).

### Objetivos del Sistema

- Recibir tramas de datos desde puerto serial USB
- Procesar comandos de rotación y carga de caracteres
- Decodificar mensajes encriptados mediante rotores configurables
- Mostrar el mensaje decodificado en consola

### Alcance

Este manual técnico está dirigido a desarrolladores y personal técnico responsable del mantenimiento, extensión o auditoría del código fuente.

---

## Arquitectura del Sistema

### Componentes Principales

El sistema se divide en los siguientes componentes:

```
┌─────────────────────────────────────────────────────────┐
│                    MAIN PROGRAM                         │
│                   (src/main.cpp)                        │
└────────────┬────────────────────────────────────────────┘
             │
             ├──────────────┬──────────────┬──────────────┐
             │              │              │              │
        ┌────▼────┐   ┌────▼────┐   ┌────▼────┐   ┌─────▼─────┐
        │ Serial  │   │  Trama  │   │  Rotor  │   │   Lista   │
        │  Port   │   │  Base   │   │   De    │   │    De     │
        │         │   │(Polim.) │   │  Mapeo  │   │   Carga   │
        └─────────┘   └────┬────┘   └─────────┘   └───────────┘
                           │
                    ┌──────┴──────┐
                    │             │
              ┌─────▼─────┐ ┌────▼─────┐
              │TramaLoad  │ │TramaMap  │
              └───────────┘ └──────────┘
```

### Patrón de Diseño

El sistema utiliza el patrón **Strategy** mediante polimorfismo:

- **TramaBase**: Clase abstracta que define la interfaz
- **TramaLoad**: Estrategia para procesar caracteres
- **TramaMap**: Estrategia para rotar el alfabeto

---

## Estructuras de Datos

### 1. Lista Doblemente Enlazada (ListaDeCarga)

**Archivo:** `include/ListaDeCarga.h`

**Propósito:** Almacenar la secuencia de caracteres decodificados del mensaje.

**Estructura del Nodo:**

```cpp
struct Nodo {
    char dato;      // Carácter decodificado
    Nodo* siguiente; // Puntero al siguiente nodo
    Nodo* anterior;  // Puntero al nodo anterior
};
```

**Complejidad:**
- Inserción al final: O(1)
- Recorrido completo: O(n)
- Destrucción: O(n)

**Métodos Principales:**

| Método | Descripción | Complejidad |
|--------|-------------|-------------|
| `insertarAlFinal(char)` | Añade un carácter al final de la lista | O(1) |
| `imprimirMensaje(Nodo*)` | Imprime recursivamente desde un nodo | O(n) |
| `~ListaDeCarga()` | Libera toda la memoria de nodos | O(n) |

### 2. Lista Circular Doblemente Enlazada (RotorDeMapeo)

**Archivo:** `include/RotorDeMapeo.h`

**Propósito:** Implementar el rotor de cifrado con el alfabeto completo A-Z.

**Estructura del Nodo:**

```cpp
struct NodoMap {
    char letra;      // Carácter del alfabeto
    NodoMap* siguiente; // Puntero al siguiente (circular)
    NodoMap* anterior;  // Puntero al anterior (circular)
};
```

**Características Especiales:**
- Lista circular: El último nodo apunta al primero
- Cabeza móvil: `rotar(n)` mueve el puntero `cabeza`
- Alfabeto completo: 26 nodos (A-Z)

**Complejidad:**
- Rotación: O(n) donde n = pasos de rotación
- Búsqueda de mapeo: O(26) = O(1)
- Inicialización: O(26) = O(1)

**Métodos Principales:**

| Método | Descripción | Parámetros | Retorno |
|--------|-------------|------------|---------|
| `rotar(int)` | Rota el alfabeto n posiciones | n: pasos (-/+) | void |
| `getMapeo(char)` | Obtiene el carácter mapeado | c: carácter original | char |
| `insertarLetra(char)` | Inserta letra en lista circular | letra: carácter | void |

**Algoritmo de Mapeo:**

```
Entrada: 'A'
Estado inicial: cabeza -> A -> B -> C -> ... -> Z -> (circular)
Después de rotar(3): cabeza -> D -> E -> F -> ... -> C -> (circular)

getMapeo('A'):
  1. Buscar 'A' desde posición original (índice 0)
  2. Desde cabeza, avanzar 0 posiciones
  3. Retornar letra en cabeza actual = 'D'
```

---

## Módulos del Sistema

### 3.1 SerialPort (Comunicación Serial)

**Archivo:** `include/SerialPort.h`

**Responsabilidad:** Gestionar la comunicación con el dispositivo Arduino/ESP32 a través del puerto USB.

**Configuración del Puerto:**
- Velocidad: 9600 baudios
- Bits de datos: 8
- Paridad: None
- Bits de parada: 1
- Control de flujo: Hardware (DTR/RTS)

**API Pública:**

```cpp
class SerialPort {
public:
    SerialPort();
    ~SerialPort();
    
    bool abrir(const char* puerto, int baudRate);
    void cerrar();
    void reiniciarDispositivo();
    int leerLinea(char* buffer, int maxLen);
};
```

**Implementación de Reinicio:**

```cpp
void reiniciarDispositivo() {
    // Activar DTR para resetear Arduino
    int status = TIOCM_DTR;
    ioctl(fd, TIOCMBIS, &status);  // Set DTR
    usleep(100000);                 // Esperar 100ms
    ioctl(fd, TIOCMBIC, &status);  // Clear DTR
    usleep(2000000);               // Esperar 2s para bootloader
}
```

**Diagrama de Estados del Puerto:**

```
[CERRADO] --abrir()--> [CONFIGURANDO] --éxito--> [ABIERTO]
                              |
                            fallo
                              |
                              v
                          [ERROR]
```

### 3.2 TramaBase (Clase Abstracta)

**Archivo:** `include/TramaBase.h`

**Propósito:** Definir la interfaz común para todos los tipos de tramas.

**Diseño Polimórfico:**

```cpp
class TramaBase {
protected:
    TramaBase();
public:
    virtual ~TramaBase();
    virtual void procesar(ListaDeCarga* carga, RotorDeMapeo* rotor) = 0;
};
```

**Principios Aplicados:**
- Abstracción: Método `procesar()` puro virtual
- Encapsulamiento: Constructor protegido
- Polimorfismo: Virtual destructor para limpieza correcta

### 3.3 TramaLoad (Carga de Carácter)

**Archivo:** `include/TramaLoad.h`

**Formato de Trama:** `L,<char>`

**Ejemplo:** `L,X`

**Algoritmo de Procesamiento:**

```
1. Extraer carácter de la trama (letraCargada)
2. Obtener carácter mapeado: mapeo = rotor->getMapeo(letraCargada)
3. Insertar en lista: carga->insertarAlFinal(mapeo)
```

**Implementación:**

```cpp
void procesar(ListaDeCarga* carga, RotorDeMapeo* rotor) override {
    char caracterMapeado = rotor->getMapeo(letraCargada);
    carga->insertarAlFinal(caracterMapeado);
}
```

### 3.4 TramaMap (Comando de Rotación)

**Archivo:** `include/TramaMap.h`

**Formato de Trama:** `M,<número>`

**Ejemplo:** `M,3` (rotar 3 posiciones adelante)

**Algoritmo de Procesamiento:**

```
1. Extraer número de pasos (pasos)
2. Aplicar rotación: rotor->rotar(pasos)
```

**Implementación:**

```cpp
void procesar(ListaDeCarga* carga, RotorDeMapeo* rotor) override {
    rotor->rotar(pasos);
}
```

---

## Protocolo de Comunicación

### Especificación PRT-7

**Formato General:**
```
<TIPO>,<DATO>\n
```

**Tipos de Tramas:**

| Tipo | Comando | Descripción | Ejemplo |
|------|---------|-------------|---------|
| L | Load | Carga un carácter | `L,A\n` |
| M | Map | Rota el alfabeto | `M,5\n` |

**Secuencia de Transmisión:**

```
Arduino                          Decodificador
  |                                    |
  |-------- M,3\n -------------------->| (Rotar 3)
  |                                    |
  |-------- L,D\n -------------------->| (Cargar D -> A)
  |                                    |
  |-------- M,-1\n ------------------->| (Rotar -1)
  |                                    |
  |-------- L,B\n -------------------->| (Cargar B -> C)
  |                                    |
```

**Validación de Tramas:**

El parser en `cargarLineas()` valida:
1. Formato correcto: `<char>,<dato>`
2. Tipo válido: 'L' o 'M'
3. Longitud mínima: 3 caracteres

**Ejemplo de Parseo:**

```cpp
// Entrada: "L,X\n"
char tipo = linea[0];           // 'L'
char dato = linea[2];           // 'X'

if (tipo == 'L') {
    return new TramaLoad(dato);  // Retorna TramaLoad*
}
```

---

## Compilación e Instalación

### Requisitos del Sistema

**Software Necesario:**
- GCC/G++ 7.0 o superior (con soporte C++11)
- CMake 3.10 o superior
- Doxygen 1.8+ (opcional, para documentación)
- Make
- Git

**Librerías del Sistema:**
- POSIX termios (comunicación serial)
- libstdc++

**Hardware:**
- Puerto USB disponible
- Arduino/ESP32 con sketch PRT-7 cargado

### Proceso de Compilación

**Paso 1: Clonar el repositorio**

```bash
git clone <URL_DEL_REPOSITORIO>
cd ds-unidad-2-actividad-2-listasdobleenlazadas-Dev-E-R
```

**Paso 2: Crear directorio de compilación**

```bash
mkdir build
cd build
```

**Paso 3: Configurar con CMake**

```bash
cmake ..
```

**Paso 4: Compilar**

```bash
cmake --build .
# O alternativamente:
make
```

**Paso 5: Instalar (requiere permisos root)**

```bash
sudo make install
```

El ejecutable se instalará en: `/usr/local/bin/decodificador`

### Estructura de Archivos CMake

**CMakeLists.txt:**

```cmake
cmake_minimum_required(VERSION 3.10)
project(Decodificador)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(decodificador src/main.cpp)

install(TARGETS decodificador DESTINATION /usr/local/bin)
```

### Verificación de la Instalación

```bash
which decodificador
# Salida esperada: /usr/local/bin/decodificador

decodificador --version  # Si implementado
```

---

## Configuración

### Configuración del Puerto Serial

**Ubicación:** `src/main.cpp` línea 51

```cpp
if (puerto.abrir("/dev/ttyUSB0", 9600)) {
    // ...
}
```

**Puertos Comunes:**

| Sistema Operativo | Puerto Típico |
|-------------------|---------------|
| Linux | `/dev/ttyUSB0`, `/dev/ttyACM0` |
| macOS | `/dev/cu.usbserial-*` |
| Windows | `COM3`, `COM4` |

**Detección Automática de Puerto:**

```bash
# Linux
ls -l /dev/ttyUSB* /dev/ttyACM*

# Información detallada
dmesg | grep tty
```

### Permisos del Puerto Serial

**Problema Común:** `Permission denied`

**Solución 1 (Temporal):**

```bash
sudo chmod 666 /dev/ttyUSB0
```

**Solución 2 (Permanente - Recomendada):**

```bash
# Añadir usuario al grupo dialout
sudo usermod -aG dialout $USER

# Cerrar sesión y volver a iniciar
```

**Verificación:**

```bash
groups
# Debe aparecer: ... dialout ...
```

### Configuración de Baudrate

El sistema está configurado para **9600 baudios**. Para cambiar:

1. Modificar en `src/main.cpp`:
   ```cpp
   puerto.abrir("/dev/ttyUSB0", 115200);  // Nuevo baudrate
   ```

2. Actualizar sketch de Arduino para coincidir:
   ```cpp
   Serial.begin(115200);
   ```

3. Recompilar ambos sistemas

---

## Diagrama de Flujo

### Flujo Principal del Programa

```
INICIO
  |
  v
Crear SerialPort, ListaDeCarga, RotorDeMapeo
  |
  v
Abrir puerto serial (/dev/ttyUSB0, 9600)
  |
  v
¿Abrió correctamente?
  |
  +-- NO --> Mostrar error --> FIN
  |
  v YES
  |
Reiniciar Arduino (pulso DTR)
  |
  v
Esperar 2 segundos (bootloader)
  |
  v
Leer 14 líneas del puerto serial
  |
  v
PARA cada línea:
  |
  +---> Parsear línea
  |       |
  |       v
  |     ¿Tipo de trama?
  |       |
  |       +-- 'L' --> Crear TramaLoad(carácter)
  |       |
  |       +-- 'M' --> Crear TramaMap(número)
  |       |
  |       v
  |     Ejecutar trama->procesar(carga, rotor)
  |       |
  |       v
  |     Eliminar trama (delete)
  |       |
  +-------+
  |
  v
Imprimir mensaje decodificado
  |
  v
Cerrar puerto serial
  |
  v
FIN
```

### Flujo de Procesamiento de TramaLoad

```
TramaLoad::procesar(carga, rotor)
  |
  v
caracterMapeado = rotor->getMapeo(letraCargada)
  |
  +---> RotorDeMapeo::getMapeo(c)
  |       |
  |       v
  |     posicionOriginal = c - 'A'  (ej: 'C' -> 2)
  |       |
  |       v
  |     actual = cabeza
  |       |
  |       v
  |     Avanzar 'posicionOriginal' nodos
  |       |
  |       v
  |     RETORNAR actual->letra
  |
  v
carga->insertarAlFinal(caracterMapeado)
  |
  +---> ListaDeCarga::insertarAlFinal(c)
  |       |
  |       v
  |     Crear nuevo Nodo(c)
  |       |
  |       v
  |     ¿Lista vacía?
  |       |
  |       +-- SÍ --> cabeza = cola = nuevo
  |       |
  |       +-- NO --> cola->siguiente = nuevo
  |                  nuevo->anterior = cola
  |                  cola = nuevo
  |
  v
RETORNAR
```

### Flujo de Procesamiento de TramaMap

```
TramaMap::procesar(carga, rotor)
  |
  v
rotor->rotar(pasos)
  |
  +---> RotorDeMapeo::rotar(n)
  |       |
  |       v
  |     ¿n > 0? (adelante)
  |       |
  |       +-- SÍ --> PARA i=0 hasta n-1:
  |       |            cabeza = cabeza->siguiente
  |       |
  |       +-- NO --> PARA i=0 hasta |n|-1:
  |                    cabeza = cabeza->anterior
  |
  v
RETORNAR
```

---

## Gestión de Memoria

### Responsabilidades de Liberación

| Clase | Recurso | Responsable | Método |
|-------|---------|-------------|--------|
| ListaDeCarga | Nodos de la lista | Destructor | `~ListaDeCarga()` |
| RotorDeMapeo | Nodos del alfabeto | Destructor | `~RotorDeMapeo()` |
| SerialPort | File descriptor | Destructor | `~SerialPort()` |
| main() | TramaBase* | main() | `delete trama;` |

### Análisis de Fugas de Memoria

**Herramienta:** Valgrind

```bash
# Compilar con símbolos de depuración
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Ejecutar con Valgrind
valgrind --leak-check=full --show-leak-kinds=all ./decodificador
```

**Resultado Esperado:**

```
HEAP SUMMARY:
    in use at exit: 0 bytes in 0 blocks
  total heap usage: X allocs, X frees, Y bytes allocated

All heap blocks were freed -- no leaks are possible
```

### Implementación de Destructores

**ListaDeCarga:**

```cpp
~ListaDeCarga() {
    Nodo* actual = cabeza;
    while (actual != nullptr) {
        Nodo* siguiente = actual->siguiente;
        delete actual;           // Liberar nodo
        actual = siguiente;
    }
}
```

**RotorDeMapeo:**

```cpp
~RotorDeMapeo() {
    if (cabeza == nullptr) return;
    
    NodoMap* actual = cabeza->siguiente;
    while (actual != cabeza) {
        NodoMap* siguiente = actual->siguiente;
        delete actual;
        actual = siguiente;
    }
    delete cabeza;  // Liberar el último nodo
}
```

---

## Resolución de Problemas

### Problema 1: Error al abrir puerto serial

**Síntoma:**
```
Error: No se pudo abrir el puerto serial
```

**Diagnóstico:**

```bash
# Verificar que el dispositivo existe
ls -l /dev/ttyUSB0

# Verificar permisos
ls -l /dev/ttyUSB0
# Debe mostrar: crw-rw---- 1 root dialout ...
```

**Soluciones:**

1. **Puerto incorrecto:** Cambiar a `/dev/ttyUSB1` o `/dev/ttyACM0`
2. **Sin permisos:** `sudo usermod -aG dialout $USER`
3. **Dispositivo ocupado:** `sudo lsof | grep ttyUSB` (cerrar programas)

### Problema 2: Segmentation Fault

**Posibles Causas:**

1. **Acceso a nullptr**
   - Verificar `cabeza != nullptr` antes de usar
   - Verificar `cola != nullptr` antes de insertar

2. **Delete duplicado**
   - Asegurar que cada `new` tiene un solo `delete`

3. **Desbordamiento de buffer**
   - `leerLinea()` usa buffer de 256 bytes
   - Arduino no debe enviar líneas mayores

**Depuración:**

```bash
# Compilar con símbolos
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Ejecutar con GDB
gdb ./decodificador
(gdb) run
(gdb) backtrace  # Cuando falle
```

### Problema 3: Mensaje decodificado incorrecto

**Verificaciones:**

1. **Orden de tramas:** Arduino debe enviar en orden correcto
2. **Baudrate:** Mismo en Arduino y decodificador (9600)
3. **Estado del rotor:** Verificar rotaciones acumuladas

**Prueba Manual:**

```cpp
// Añadir al final de main() antes de imprimir:
std::cout << "Rotaciones totales aplicadas: " << contadorRotaciones << std::endl;
```

### Problema 4: Documentación Doxygen vacía

**Solución:**

Verificar `Doxyfile`:
```
INPUT = include src
RECURSIVE = YES
SOURCE_BROWSER = YES
INLINE_SOURCES = YES
```

Regenerar:
```bash
doxygen Doxyfile
```

---

## Especificaciones Técnicas

### Límites del Sistema

| Parámetro | Valor | Unidad |
|-----------|-------|--------|
| Tamaño máximo de mensaje | 256 | caracteres |
| Longitud de línea serial | 256 | bytes |
| Número de tramas procesadas | 14 | tramas |
| Tamaño del alfabeto | 26 | letras |
| Velocidad de comunicación | 9600 | baudios |
| Timeout de lectura serial | 5 | segundos |

### Complejidad Computacional

| Operación | Complejidad | Justificación |
|-----------|-------------|---------------|
| Inicialización del rotor | O(26) = O(1) | Alfabeto fijo |
| Rotación del rotor | O(n) | n = pasos de rotación |
| Mapeo de carácter | O(26) = O(1) | Búsqueda lineal acotada |
| Inserción en lista | O(1) | Inserción al final con puntero cola |
| Impresión de mensaje | O(m) | m = longitud del mensaje |
| **Total por trama Load** | **O(1)** | Todas operaciones constantes |
| **Total por trama Map** | **O(n)** | n = pasos de rotación |

### Espacio en Memoria

**Uso estático:**
- Rotor: 26 nodos × (1 char + 2 punteros) = 26 × 17 bytes ≈ 442 bytes
- Buffer serial: 256 bytes

**Uso dinámico:**
- Lista de carga: m nodos × 17 bytes (m = longitud mensaje)
- Tramas: 14 × sizeof(TramaBase*) = 14 × 8 = 112 bytes

**Total estimado:** < 2 KB para mensajes típicos

### Compatibilidad de Plataformas

| Plataforma | Soportada | Notas |
|------------|-----------|-------|
| Linux (x86_64) | Sí | Completamente probado |
| Linux (ARM) | Sí | Raspberry Pi compatible |
| macOS | Parcial | Cambiar a `/dev/cu.usbserial-*` |
| Windows | No | Requiere adaptación a WinAPI |

---

## Apéndices

### Apéndice A: Ejemplo de Sesión Completa

**Entrada desde Arduino:**

```
M,3
L,D
L,H
M,-1
L,B
L,O
M,2
L,C
L,F
L,L
L,C
M,1
L,B
L,T
L,D
```

**Procesamiento:**

```
1. M,3  -> Rotar +3: [A..Z] -> [D..C]
2. L,D  -> Mapeo: D -> A
3. L,H  -> Mapeo: H -> E
4. M,-1 -> Rotar -1: [D..C] -> [C..B]
5. L,B  -> Mapeo: B -> C
6. L,O  -> Mapeo: O -> M
7. M,2  -> Rotar +2: [C..B] -> [E..D]
8. L,C  -> Mapeo: C -> I
9. L,F  -> Mapeo: F -> L
...
```

**Salida:**

```
=== Decodificador  PRT-7 ===

Esperando datos de Arduino...
Mensaje decodificado: AECMIL...
```

### Apéndice B: Comandos Útiles

**Monitorear puerto serial:**

```bash
# Ver datos crudos
cat /dev/ttyUSB0

# Usar screen (Ctrl+A K para salir)
screen /dev/ttyUSB0 9600

# Usar minicom
minicom -D /dev/ttyUSB0 -b 9600
```

**Limpiar compilación:**

```bash
cd build
make clean
# O eliminar todo:
rm -rf build/*
```

**Generar documentación:**

```bash
doxygen Doxyfile
xdg-open docs/html/index.html
```

### Apéndice C: Referencias

**Estándares:**
- ISO/IEC 14882:2011 (C++11)
- POSIX.1-2008 (termios)

**Librerías:**
- GNU C++ Library (libstdc++)
- POSIX Threads

**Herramientas:**
- CMake Documentation: https://cmake.org/documentation/
- Doxygen Manual: https://www.doxygen.nl/manual/

---

## Historial de Versiones

| Versión | Fecha | Autor | Cambios |
|---------|-------|-------|---------|
| 1.0 | 06/11/2025 | Dev-E-R | Versión inicial del manual técnico |

---

**Fin del Manual Técnico**
