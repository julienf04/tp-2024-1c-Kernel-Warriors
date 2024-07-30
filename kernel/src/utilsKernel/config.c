#include "config.h"
#include <commons/config.h>
#include "logger.h"
#include "utils/config.h"
#include <stdio.h>
#include <stdlib.h>



kernelConfig* _kernelConfig;
t_config* _configFile;


kernelConfig* getKernelConfig()
{
    return _kernelConfig;
}

// FUncion auxiliar para settear todos los datos desde el config hasta la variable global correspondiente
void _getKernelData()
{
    getKernelConfig()->PUERTO_ESCUCHA = config_get_string_value(_configFile, "PUERTO_ESCUCHA");

    getKernelConfig()->IP_MEMORIA = config_get_string_value(_configFile, "IP_MEMORIA");

    getKernelConfig()->PUERTO_MEMORIA = config_get_string_value(_configFile, "PUERTO_MEMORIA");

    getKernelConfig()->IP_CPU = config_get_string_value(_configFile, "IP_CPU");

    getKernelConfig()->PUERTO_CPU_DISPATCH = config_get_string_value(_configFile, "PUERTO_CPU_DISPATCH");

    getKernelConfig()->PUERTO_CPU_INTERRUPT = config_get_string_value(_configFile, "PUERTO_CPU_INTERRUPT");

    getKernelConfig()->ALGORITMO_PLANIFICACION = config_get_string_value(_configFile, "ALGORITMO_PLANIFICACION");

    getKernelConfig()->QUANTUM = config_get_int_value(_configFile, "QUANTUM");

    getKernelConfig()->RECURSOS = getListOfStringsFromConfig(_configFile, "RECURSOS", getLogger(), "No se pudieron obtener los recursos del archivo de configuracion");

    getKernelConfig()->INSTANCIAS_RECURSOS = getListOfIntsFromConfig(_configFile, "INSTANCIAS_RECURSOS", getLogger(), "No se pudo obtener la cantidad de instancias de los recursos del archivo de configuracion");

    getKernelConfig()->GRADO_MULTIPROGRAMACION = config_get_int_value(_configFile, "GRADO_MULTIPROGRAMACION");
}


void initKernelConfig(char* path)
{
    //log_info(getLogger(), "Obteniendo los datos del archivo de configuracion");

    _configFile = config_create(path);

    if (_configFile == NULL){
        log_info(getLogger(), "Fallo en la creación del config");
        exit(1);
    }

    _kernelConfig = (kernelConfig*) malloc(sizeof(kernelConfig));

    _getKernelData();

    //log_info(getLogger(), "Datos obtenidos con exito");
}



// Posible riesgo de memory leak en esta funcion. No deberia haber ninguno, pero revisar esta funcion en caso de que se detecte un memory leak en los testeos.
void freeKernelConfig()
{
    //log_info(getLogger(), "Liberando la memoria usada para el archivo de configuracion");

    list_destroy_and_destroy_elements(getKernelConfig()->RECURSOS, free);
    list_destroy_and_destroy_elements(getKernelConfig()->INSTANCIAS_RECURSOS, free);
    config_destroy(_configFile);
    free(getKernelConfig());

    //log_info(getLogger(), "Memoria liberada con exito");
}

