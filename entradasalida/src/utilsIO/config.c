#include "config.h"
#include <commons/config.h>
#include "logger.h"
#include "utils/config.h"
#include <stdio.h>
#include <stdlib.h>



ioConfig* _ioConfig;
t_config* _configFile;


ioConfig* getIOConfig()
{
    return _ioConfig;
}

// Funcion auxiliar para settear todos los datos desde el config hasta la variable global correspondiente
void _getIOData()
{
    getIOConfig()->TIPO_INTERFAZ = config_get_string_value(_configFile, "TIPO_INTERFAZ");

    if (config_has_property(_configFile, "TIEMPO_UNIDAD_TRABAJO"))
        getIOConfig()->TIEMPO_UNIDAD_TRABAJO = config_get_int_value(_configFile, "TIEMPO_UNIDAD_TRABAJO");

    if (config_has_property(_configFile, "IP_KERNEL"))
        getIOConfig()->IP_KERNEL = config_get_string_value(_configFile, "IP_KERNEL");

    if (config_has_property(_configFile, "PUERTO_KERNEL"))
        getIOConfig()->PUERTO_KERNEL = config_get_string_value(_configFile, "PUERTO_KERNEL");

    if (config_has_property(_configFile, "IP_MEMORIA"))
        getIOConfig()->IP_MEMORIA = config_get_string_value(_configFile, "IP_MEMORIA");

    if (config_has_property(_configFile, "PUERTO_MEMORIA"))
        getIOConfig()->PUERTO_MEMORIA = config_get_string_value(_configFile, "PUERTO_MEMORIA");

    if (config_has_property(_configFile, "PATH_BASE_DIALFS"))
        getIOConfig()->PATH_BASE_DIALFS = config_get_string_value(_configFile, "PATH_BASE_DIALFS");

    if (config_has_property(_configFile, "BLOCK_SIZE"))
        getIOConfig()->BLOCK_SIZE = config_get_int_value(_configFile, "BLOCK_SIZE");

    if (config_has_property(_configFile, "BLOCK_COUNT"))
        getIOConfig()->BLOCK_COUNT = config_get_int_value(_configFile, "BLOCK_COUNT");

    if (config_has_property(_configFile, "RETRASO_COMPACTACION"))
        getIOConfig()->RETRASO_COMPACTACION = config_get_int_value(_configFile, "RETRASO_COMPACTACION");
}

void initIOConfig(char* path)
{
    _configFile = config_create(path);

    _ioConfig = (ioConfig*) malloc(sizeof(ioConfig));

    _getIOData();
}

// Posible riesgo de memory leak o de Segmentation Fault en esta funcion. No deberia haber ninguno, pero revisar esta funcion en caso de que se detecte un memory leak o un Segmentation Fault en los testeos
void freeIOConfig()
{
    config_destroy(_configFile);
    free(getIOConfig());
}
