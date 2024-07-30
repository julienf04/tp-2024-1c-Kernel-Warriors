#ifndef KERNEL_LOGGER_H_
#define KERNEL_LOGGER_H_


#include <commons/log.h>



// Inicializa el logger
void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level);

// Retorna el logger de este modulo
t_log* getLogger();

// Destruye el logger
void destroyLogger();



#endif