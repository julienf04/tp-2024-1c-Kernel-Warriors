#ifndef MEMORY_LOGGER_H_
#define MEMORY_LOGGER_H_


#include <commons/log.h>
#include "memoryDebug.h"



///////////////////// FUNCIONES PARA MANEJO DE LOGS /////////////////////


// Inicializa el logger
void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level);

// Retorna el logger de este modulo
t_log* getLogger();

// Destruye el logger
void destroyLogger();




///////////////////// LOGS ESPECIFICOS /////////////////////


void logCreateProcess(int pid, int amountOfPages);

void logCreateProcessError(char* pseudocodePath);

void logDestroyProcess(int pid, int amountOfPages);

void logPageTableAccess(int pid, int page, int frame);

void logProcessSizeExpansion(int pid, int currentSize, int sizeToExpand);

void logProcessSizeReduction(int pid, int currentSize, int sizeToReduct);

void logProcessSizeNotChange(int pid, int currentSize);

void logReadBytes(int pid, int physicalAddress, int size);

void logWriteBytes(int pid, int physicalAddress, int size);




////////////////////// LOGS AUXILIARES GENERALES //////////////////////



#ifndef LINE_SEPARATOR
#define LINE_SEPARATOR "----------------------------------------------------"
#endif


void logLineSeparator();




////////////////////// LOGS ESPECIFICOS PARA EL DEBUG O REALESE //////////////////////


#ifdef DEBUG_MEMORY // PARA EL DEBUG


void logPreInitialMessageDebug();

void logInitialMessageDebug();


#else // PARA EL REALESE


void logInitialMessageRealese();



#endif



#endif