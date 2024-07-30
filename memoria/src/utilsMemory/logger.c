#include "logger.h"

t_log* _logger;



///////////////////// FUNCIONES PARA MANEJO DE LOGS /////////////////////


void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level)
{
    _logger = log_create(file, processName, isActiveConsole, level);
}

t_log* getLogger()
{
    return _logger;
}

void destroyLogger()
{
    log_destroy(_logger);
}



///////////////////// LOGS ESPECIFICOS /////////////////////


void logCreateProcess(int pid, int amountOfPages)
{
    log_info(getLogger(), "PID: %d - Creación - Tamaño: %d", pid, amountOfPages);
}

void logCreateProcessError(char* pseudocodePath)
{
    log_info(getLogger(), "No se pudo abrir el archivo \"%s\".", pseudocodePath);
}

void logDestroyProcess(int pid, int amountOfPages)
{
    log_info(getLogger(), "PID: %d - Destrucción - Tamaño: %d", pid, amountOfPages);
}

void logPageTableAccess(int pid, int page, int frame)
{
    log_info(getLogger(), "PID: %d - Pagina: %d - Marco: %d", pid, page, frame);
}

void logProcessSizeExpansion(int pid, int currentSize, int sizeToExpand)
{
    log_info(getLogger(), "PID: %d - Tamaño Actual: %d - Tamaño a Ampliar: %d", pid, currentSize, sizeToExpand);
}

void logProcessSizeReduction(int pid, int currentSize, int sizeToReduct)
{
    log_info(getLogger(), "PID: %d - Tamaño Actual: %d - Tamaño a Reducir: %d", pid, currentSize, sizeToReduct);
}

void logProcessSizeNotChange(int pid, int currentSize)
{
    log_info(getLogger(), "PID: %d - Tamaño Actual: %d - Tamaño a Ampliar/Reducir: %d", pid, currentSize, 0);
}

void logReadBytes(int pid, int physicalAddress, int size)
{
    log_info(getLogger(), "PID: %d - Accion: LEER - Direccion fisica: %d - Tamaño: %d", pid, physicalAddress, size);
}

void logWriteBytes(int pid, int physicalAddress, int size)
{
    log_info(getLogger(), "PID: %d - Accion: ESCRIBIR - Direccion fisica: %d - Tamaño: %d", pid, physicalAddress, size);
}




////////////////////// LOGS AUXILIARES GENERALES //////////////////////



void logLineSeparator()
{
    log_info(getLogger(), LINE_SEPARATOR);
}



////////////////////// LOGS ESPECIFICOS PARA EL DEBUG O REALESE //////////////////////


#ifdef DEBUG_MEMORY


void logPreInitialMessageDebug()
{
    log_info(getLogger(), "INICIANDO EL MODULO MEMORIA EN MODO DEBUG");
}

void logInitialMessageDebug()
{
    log_info(getLogger(), "INICIADO EL MODULO MEMORIA EN MODO DEBUG");
}


#else


void logInitialMessageRealese()
{
    logLineSeparator();
    log_info(getLogger(), "INICIADO EL MODULO MEMORIA");
    logLineSeparator();
}


#endif