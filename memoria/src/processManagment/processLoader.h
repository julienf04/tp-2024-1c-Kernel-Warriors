#ifndef MEMORY_PROCESS_LOADER_H_
#define MEMORY_PROCESS_LOADER_H_


#include <stdio.h>
#include <stdlib.h>
#include "essentials.h"


/// @brief Inicializa la lista de los procesos.
void initProcessesList();


/// @brief Carga un proceso, que consiste en cargar las instrucciones de pseudocodigo
/// en la lista de pseudocodigos de procesos.
/// @param PID Process ID, necesario para identificar al proceso.
/// @param pseudocodePath El path donde se encuentra el archivo de pseudocodigo cargado en disco.
/// @return Retorna true si el proceso pudo crearse correctamente, sino false.
bool loadProcessByPath(int PID, char* pseudocodePath);



/// @brief Carga un proceso, que consiste en cargar las instrucciones de pseudocodigo
/// en la lista de procesos e inicializar su tabla de paginas. No cierra el archivo pasado como parametro.
/// @param PID Process ID, necesario para identificar al proceso.
/// @param pseudocodePath El archivo con el pseudocodigo.
void loadProcessByFile(int PID, FILE* pseudocodeFile);


/// @brief Llama a la funcion que carga un proceso usando su path. La unica diferencia es que recibe los parametros usando un void* en lugar de varios parametros.
/// @param params Los parametros void*
void loadProcessByPathWithParams(void* params);


/// @brief Elimina la informacion del proceso con el PID especificado (lo elimina de la lista y libera la memoria)
/// @param PID El Process ID
void destroyProcess(int PID);


/// @brief Llama a la funcion que elimina la informacion del pseudocodigo con el PID especificado pasado mediante params
/// @param params El PID en forma de void*
void destroyProcessWithParams(void* params);






#endif