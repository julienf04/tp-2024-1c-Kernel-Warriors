#ifndef CPU_CHECK_INTERRUPT_H_
#define CPU_CHECK_INTERRUPT_H_

#include "essentials.h"
#include "utils/listMutex.h"
#include "utils/utilsGeneral.h"
#include <commons/collections/list.h>


// Lista que contiene la informacion de las interrupciones que tengo que resolver
extern listMutex_t* interruptionsList;



typedef enum
{
    END_QUANTUM_TYPE,
    END_PROCESS_TYPE,
} interruptionType;




typedef struct
{
    int pid;
    interruptionType type;
} interruptionInfo;



/// @brief Inicializa las interrupciones
void initInterrupts();


/// @brief Agrega una interrupcion al final de la lista de interrupciones
/// @param info La informacion de la interrupcion. Debe ser un puntero valido y que apunte a los datos correctos. El puntero debe haberse reservado usando malloc
void addInterruption(interruptionInfo* info);


/// @brief Chequea las interrupciones
void checkInterrupts();


/// @brief Atiende el tipo de interrupcion pasada
/// @param type El tipo de interrupcion
void attendInterruption(interruptionType type);



/// @brief Atiende la interrupcion por fin de quantum.
void attendInterruptionEndQuantum();


/// @brief Antiende la interrupcion por fin del proceso.
void attendInterruptionEndProcess();









#endif