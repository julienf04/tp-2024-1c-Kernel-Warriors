#ifndef CPU_LOGGER_H_
#define CPU_LOGGER_H_


#include <commons/log.h>
#include <stdint.h>
#include "cpuDebug.h"



///////////////////// FUNCIONES PARA MANEJO DE LOGS /////////////////////


// Inicializa el logger
void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level);

// Retorna el logger de este modulo
t_log* getLogger();

// Destruye el logger
void destroyLogger();




///////////////////// LOGS ESPECIFICOS /////////////////////


void logFetchInstruction(int pid, uint32_t pc);

void logExecutingInstruction(int pid, char* instruction);

void logTLBHit(int pid, int page);

void logTLBMiss(int pid, int page);

void logGetFrame(int pid, int page, int frame);

void logReadMemoryUint(int pid, int amountOfPhysicalAddresses, int physicalAddresses[], uint32_t value);

void logReadMemoryString(int pid, int amountOfPhysicalAddresses, int physicalAddresses[], char* value);

void logWriteMemoryUint(int pid, int amountOfPhysicalAddresses, int physicalAddresses[], uint32_t value);

void logWriteMemoryString(int pid, int amountOfPhysicalAddresses, int physicalAddresses[], char* value);




////////////////////// LOGS AUXILIARES GENERALES //////////////////////



#ifndef LINE_SEPARATOR
#define LINE_SEPARATOR "----------------------------------------------------"
#endif


void logLineSeparator();




////////////////////// LOGS ESPECIFICOS PARA EL DEBUG O REALESE //////////////////////


#ifdef DEBUG_CPU


void logPreInitialMessageDebug();

void logInitialMessageDebug();


#else


void logInitialMessageRealese();



#endif



////////////////////// FUNCIONES AUXILIARES //////////////////////


/// @brief Convierte un array de direcciones fisicas en un string, con el siguiente formato (los numeros son de ejemplo): 0, 3, 7, 4, 2, 10
/// @param amountOfPhysicalAddresses La cantidadd de direcciones fisicas
/// @param physicalAddresses El array de direcciones fisicas
/// @return Retorna el string. Es necesario hacerle un free despues de usarlo.
/// @warning Es necesario liberar la memoria del string una vez usado.
char* physicalAddressesToString(int amountOfPhysicalAddresses, int physicalAddresses[]);





#endif