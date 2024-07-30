#ifndef UTILS_LIST_MUTEX_H
#define UTILS_LIST_MUTEX_H

#include <commons/collections/list.h>
#include <pthread.h>
#include <stdlib.h>
#include "debugMode.h"


// Lista que solo puede ser accedida o modificada por un hilo o proceso a la vez/
typedef struct 
{
	t_list *list;
	pthread_mutex_t mutex;
} listMutex_t;

bool list_remove_element_mutex(listMutex_t *list, void* element);

int list_mutex_size(listMutex_t *list);

bool list_mutex_is_empty(listMutex_t *list);

/// @brief Retorna el primer valor encontrado que haga que la condicion retorne true
/// @param list La lista mutex
/// @param closure La condicion que debe retornar true
/// @return Retorna el primer valor encontrado que cumpla con la condicion.
void* list_find_mutex(listMutex_t* list, bool(*closure)(void*));


/// @brief Agrega un elemento al final de la lista.
/// @param list La lista.
/// @param info El elemento a agregar.
void list_push(listMutex_t *list, void *info);


/// @brief Remueve el primer elemento de la lista y lo retorna
/// @param list La lista
/// @return Retorna el elemento eliminado.
void* list_pop(listMutex_t *list);



/// @brief Crea e inicializa una lista mutex.
/// @return Retorna la lista mutex inicializada.
listMutex_t* initListMutex();



/// @brief Remueve el primer elemento de la lista que haga que condition devuelva true.
/// @param self La lista.
/// @param condition La condicion que debe cumplirse para remover el elemento.
/// @return Retorna el campo data del elemento destruido (solo se destruye el nodo de la lista, no la informacion que contiene)
void* list_remove_by_condition_mutex(listMutex_t *list, bool(*condition)(void*));



/// @brief Retorna true si la lista mutex está vacía, y false si no lo está
/// @param list La lista mutex
/// @return Retorna true si la lista mutex está vacía, y false si no lo está
bool list_is_empty_mutex(listMutex_t* list);




/// @brief Destruye una lista y su mutex sin liberar los elementos contenidos en los nodos
/// @param list La lista.
void destroyListMutex(listMutex_t* list);

#endif