#ifndef KERNEL_CONFIG_H_
#define KERNEL_CONFIG_H_

#include <commons/collections/list.h>
#include <commons/config.h>

// Configuracion del archivo de configuracion del kernel
typedef struct
{
    char* PUERTO_ESCUCHA;
    char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    char* IP_CPU;
    char* PUERTO_CPU_DISPATCH;
    char* PUERTO_CPU_INTERRUPT;
    char* ALGORITMO_PLANIFICACION;
    int QUANTUM;
    t_list* RECURSOS; // Lista que almacena el tipo de dato "int"
    t_list* INSTANCIAS_RECURSOS; // Lista que almacena "char*" (es decir, strings)
    int GRADO_MULTIPROGRAMACION;
} kernelConfig;




// Retorna un puntero a la struct propia de la configuracion, la cual deberia contener la informacion de la configuracion del modulo en el que se encuentre. Llame a la funcion init propia de cada modulo antes de usar esta funcion.
kernelConfig* getKernelConfig();


// Obtiene la configuracion del archivo de configuracion especificado en el parametro "path", y los settea en una variable.
// Para obtener esa variable, llame a la funcion get propio de cada modulo. Debe llamar a esta funcion una sola vez en todo el programa, excepto que el archivo de configuracion se modifique en tiempo de ejecucion (que no deberia ocurrir).
// Loggea con log_info lo que va ocurriendo.
void initKernelConfig(char* path);


// Libera la memoria usada que almacenaba la configuracion del modulo en el que se esta parado.
// Loggea con log_info lo que va ocurriendo.
void freeKernelConfig();


#endif