#ifndef CPU_DEBUG_UTILS_H_
#define CPU_DEBUG_UTILS_H_


#include "cpuDebug.h"


#ifdef DEBUG_CPU

#include "instructionCycle/essentials.h"



/// @brief Muestra todos los registros.
void printRegisters();


/// @brief Thread que muestra cierta informacion segun los comandos escritos. Por ejemplo, uno de los comandos es "help", que da ayuda sobre la funcionalidad.
void printRegistersThread(void* ignore);

#endif








#endif