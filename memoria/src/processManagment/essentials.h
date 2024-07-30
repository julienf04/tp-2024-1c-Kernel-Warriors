#ifndef MEMORY_PROCESS_MANAGMENT_ESSENTIALS_H_
#define MEMORY_PROCESS_MANAGMENT_ESSENTIALS_H_


#include <stdint.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <semaphore.h>
#include "utils/listMutex.h"
#include "utilsMemory/config.h"





// Lista que contiene toda la info de los procesos, tanto su PID, su tabla de paginas como sus instrucciones de pseudocodigo.
extern listMutex_t* processesList;



// Estructura que contiene informacion del pseudocodigo perteneciente a un proceso (identificado con su PID)
typedef struct
{
    int PID;
    char** pseudocodeInstructions;
    int* pageTable;
    int amountOfPages;
    int internalFragmentation; // Cantidad de btyes que representa la fragmentacion interna que tiene la ultima pagina
} processInfo;





/// @brief Obtiene la informacion del pseudocodigo que estaba corriendo en la instruccion anterior
/// @return La informacion del pseudocodigo
/// @note Es como si fuese una cache. Lo mas probable es que la siguiente instruccion que se pida sea del mismo archivo de pseudocodigo.
/// Si no lo guardo, tendria que buscar el archivo con el PID entre la lista de todos los procesos, entonces lo guardo para que sea más rápido.
/// En caso de que el PID no sea el mismo que el de la instruccion anterior, sí necesitaria buscar el proceso dentro de la lista de pseudocodigos de los procesos.
processInfo* getLastProcess();


/// @brief Settea el valor a la variable que contiene informacion del pseudocodigo del proceso actual.
/// @param value El nuevo valor.
void setLastProcess(processInfo* value);



extern sem_t semAuxPID;

extern int auxPID;

/// @brief Retorna true si el PID pasado como parametro es igual al valor del PID auxiliar.
/// @param element El PID a comparar con el PID auxiliar.
/// @return Retorna true si ambos PIDs son iguales.
bool closurePIDsAreEqual(void* element);






#endif