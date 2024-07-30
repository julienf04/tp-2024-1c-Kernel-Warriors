#ifndef MEMORY_CONFIG_H_
#define MEMORY_CONFIG_H_

#include <commons/collections/list.h>
#include <commons/config.h>

// Configuracion del archivo de configuracion de la memoria
typedef struct
{
    char* PUERTO_ESCUCHA;
    int TAM_MEMORIA;
    int TAM_PAGINA;
    char* PATH_INSTRUCCIONES;
    int RETARDO_RESPUESTA;
} memoryConfig;




// Retorna un puntero a la struct propia de la configuracion, la cual deberia contener la informacion de la configuracion del modulo en el que se encuentre. Llame a la funcion init propia de cada modulo antes de usar esta funcion.
memoryConfig* getMemoryConfig();


// Obtiene la configuracion del archivo de configuracion especificado en el parametro "path", y los settea en una variable.
// Para obtener esa variable, llame a la funcion get propio de cada modulo. Debe llamar a esta funcion una sola vez en todo el programa, excepto que el archivo de configuracion se modifique en tiempo de ejecucion (que no deberia ocurrir).
// Loggea con log_info lo que va ocurriendo.
void initMemoryConfig(char* path);


// Libera la memoria usada que almacenaba la configuracion del modulo en el que se esta parado.
// Loggea con log_info lo que va ocurriendo.
void freeMemoryConfig();



#endif