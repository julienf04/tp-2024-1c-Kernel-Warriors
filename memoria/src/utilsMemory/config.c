#include "config.h"
#include <commons/config.h>
#include "logger.h"
#include "utils/config.h"
#include <stdio.h>
#include <stdlib.h>
#include "memoryDebug.h"



memoryConfig* _memoryConfig;
t_config* _configFile;


memoryConfig* getMemoryConfig()
{
    return _memoryConfig;
}

// FUncion auxiliar para settear todos los datos desde el config hasta la variable global correspondiente
void _getMemoryData()
{
    getMemoryConfig()->PUERTO_ESCUCHA = config_get_string_value(_configFile, "PUERTO_ESCUCHA");

    getMemoryConfig()->TAM_MEMORIA = config_get_int_value(_configFile, "TAM_MEMORIA");

    getMemoryConfig()->TAM_PAGINA = config_get_int_value(_configFile, "TAM_PAGINA");

    getMemoryConfig()->PATH_INSTRUCCIONES = config_get_string_value(_configFile, "PATH_INSTRUCCIONES");

    getMemoryConfig()->RETARDO_RESPUESTA = config_get_int_value(_configFile, "RETARDO_RESPUESTA");
}

void initMemoryConfig(char* path)
{
    #ifdef DEBUG_MEMORY
    log_info(getLogger(), "Obteniendo los datos del archivo de configuracion");
    #endif

    _configFile = config_create(path);

    _memoryConfig = (memoryConfig*) malloc(sizeof(memoryConfig));

    _getMemoryData();

    #ifdef DEBUG_MEMORY
    log_info(getLogger(), "Datos obtenidos con exito");
    #endif
}

// Posible riesgo de memory leak o de Segmentation Fault en esta funcion. No deberia haber ninguno, pero revisar esta funcion en caso de que se detecte un memory leak o un Segmentation Fault en los testeos
void freeMemoryConfig()
{
    #ifdef DEBUG_MEMORY
    log_info(getLogger(), "Liberando la memoria usada para el archivo de configuracion");
    #endif

    config_destroy(_configFile);
    free(getMemoryConfig());

    #ifdef DEBUG_MEMORY
    log_info(getLogger(), "Memoria liberada con exito");
    #endif
}


