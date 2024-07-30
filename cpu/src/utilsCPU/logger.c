#include "logger.h"
#include <stdint.h>
#include <commons/string.h>

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


void logFetchInstruction(int pid, uint32_t pc)
{
    log_info(getLogger(), "PID: %d - FETCH - Program Counter: %d", pid, pc);
}

void logExecutingInstruction(int pid, char* instruction)
{
    log_info(getLogger(), "PID: %d - Ejecutando: %s", pid, instruction);
}

void logTLBHit(int pid, int page)
{
    log_info(getLogger(), "PID: %d - TLB HIT - Pagina: %d", pid, page);
}

void logTLBMiss(int pid, int page)
{
    log_info(getLogger(), "PID: %d - TLB MISS - Pagina: %d", pid, page);
}

void logGetFrame(int pid, int page, int frame)
{
    log_info(getLogger(), "PID: %d - OBTENER MARCO - Página: %d - Marco: %d", pid, page, frame);
}

void logReadMemoryUint(int pid, int amountOfPhysicalAddresses, int physicalAddresses[], uint32_t value)
{
    char* stringPhysicalAddresses = physicalAddressesToString(amountOfPhysicalAddresses, physicalAddresses);
    log_info(getLogger(), "PID: %d - Acción: LEER - Direcciones Físicas: %s - Valor: %u", pid, stringPhysicalAddresses, value);
    free(stringPhysicalAddresses);
}

void logReadMemoryString(int pid, int amountOfPhysicalAddresses, int physicalAddresses[], char* value)
{
    char* stringPhysicalAddresses = physicalAddressesToString(amountOfPhysicalAddresses, physicalAddresses);
    log_info(getLogger(), "PID: %d - Acción: LEER - Direcciones Físicas: %s - Valor: %s", pid, stringPhysicalAddresses, value);
    free(stringPhysicalAddresses);
}

void logWriteMemoryUint(int pid, int amountOfPhysicalAddresses, int physicalAddresses[], uint32_t value)
{
    char* stringPhysicalAddresses = physicalAddressesToString(amountOfPhysicalAddresses, physicalAddresses);
    log_info(getLogger(), "PID: %d - Acción: ESCRIBIR - Direcciones Físicas: %s - Valor: %u", pid, stringPhysicalAddresses, value);
    free(stringPhysicalAddresses);
}

void logWriteMemoryString(int pid, int amountOfPhysicalAddresses, int physicalAddresses[], char* value)
{
    char* stringPhysicalAddresses = physicalAddressesToString(amountOfPhysicalAddresses, physicalAddresses);
    log_info(getLogger(), "PID: %d - Acción: ESCRIBIR - Direcciones Físicas: %s - Valor: %s", pid, stringPhysicalAddresses, value);
    free(stringPhysicalAddresses);
}





////////////////////// LOGS AUXILIARES GENERALES //////////////////////



void logLineSeparator()
{
    log_info(getLogger(), LINE_SEPARATOR);
}



////////////////////// LOGS ESPECIFICOS PARA EL DEBUG O REALESE //////////////////////


#ifdef DEBUG_CPU


void logPreInitialMessageDebug()
{
    log_info(getLogger(), "INICIANDO EL MODULO CPU EN MODO DEBUG");
}

void logInitialMessageDebug()
{
    log_info(getLogger(), "INICIADO EL MODULO CPU EN MODO DEBUG");
}


#else


void logInitialMessageRealese()
{
    logLineSeparator();
    log_info(getLogger(), "INICIADO EL MODULO CPU");
    logLineSeparator();
}


#endif


////////////////////// FUNCIONES AUXILIARES //////////////////////

char* physicalAddressesToString(int amountOfPhysicalAddresses, int physicalAddresses[])
{
    char* string = string_new();
    char* currentPhysicalAddressString;

    currentPhysicalAddressString = string_itoa(physicalAddresses[0]);
    string_append(&string, currentPhysicalAddressString);
    free(currentPhysicalAddressString);

    for (int i = 1; i < amountOfPhysicalAddresses; i++)
    {
        currentPhysicalAddressString = string_itoa(physicalAddresses[i]);
        string_append(&string, ", ");
        string_append(&string, currentPhysicalAddressString);
        free(currentPhysicalAddressString);
    }
    
    return string;
}