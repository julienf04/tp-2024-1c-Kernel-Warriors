#ifndef MEMORY_DEBUG_UTILS_H_
#define MEMORY_DEBUG_UTILS_H_


#include "memoryDebug.h"



#ifdef DEBUG_MEMORY



/// @brief Muestra el valor actual de toda la memoria.
void printMemory();


/// @brief Muestra el valor actual de toda la memoria mostrando los caracteres en ASCII de cada posicion de memoria.
void printMemoryChars();


/// @brief Thread que muestra cierta informacion segun los comandos escritos. Por ejemplo, uno de los comandos es "help", que da ayuda sobre la funcionalidad.
void printMemoryThread(void* ignore);


#endif





#endif