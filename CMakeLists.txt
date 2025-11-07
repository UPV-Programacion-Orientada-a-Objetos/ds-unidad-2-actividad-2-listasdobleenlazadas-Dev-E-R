cmake_minimum_required(VERSION 3.10)
project(DecodificadorPRT7 VERSION 1.0)

# Establecer el estándar de C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Incluir directorios de headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Agregar el ejecutable
add_executable(decodificador src/main.cpp)

# Configuración para instalación
install(TARGETS decodificador DESTINATION bin)

# Mensaje de información
message(STATUS "Configurando Decodificador PRT-7...")
message(STATUS "Versión: ${PROJECT_VERSION}")
message(STATUS "Estándar C++: ${CMAKE_CXX_STANDARD}")
