#ifndef IO_CONFIG_H_
#define IO_CONFIG_H_

#include <commons/collections/list.h>
#include <commons/config.h>

// Configuracion del archivo de configuracion de la entrada/salida
typedef struct
{
    char* TIPO_INTERFAZ;
    int TIEMPO_UNIDAD_TRABAJO;
    char* IP_KERNEL;
    char* PUERTO_KERNEL;
    char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    char* PATH_BASE_DIALFS;
    int BLOCK_SIZE;
    int BLOCK_COUNT;
    int RETRASO_COMPACTACION;
} ioConfig;


// Retorna un puntero a la struct propia de la configuracion, la cual deberia contener la informacion de la configuracion del modulo en el que se encuentre. Llame a la funcion init propia de cada modulo antes de usar esta funcion.
ioConfig* getIOConfig();

// Funcion auxiliar para settear todos los datos desde el config hasta la variable global correspondiente
void _getIOData();

// Obtiene la configuracion del archivo de configuracion especificado en el parametro "path", y los settea en una variable.
// Para obtener esa variable, llame a la funcion get propio de cada modulo. Debe llamar a esta funcion una sola vez en todo el programa, excepto que el archivo de configuracion se modifique en tiempo de ejecucion (que no deberia ocurrir).
// Loggea con log_info lo que va ocurriendo.
void initIOConfig(char* path);


// Libera la memoria usada que almacenaba la configuracion del modulo en el que se esta parado.
// Loggea con log_info lo que va ocurriendo.
void freeIOConfig();

//ioConfigGeneric *getIOConfigGeneric(ioConfig *config);

#endif