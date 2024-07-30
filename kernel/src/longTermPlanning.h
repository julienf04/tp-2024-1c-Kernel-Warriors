#ifndef LONG_TERM_PLANNING_H_
#define LONG_TERM_PLANNING_H_

#include "utils/listMutex.h"
#include "utils/utilsGeneral.h"
#include "generalPlanning.h"
#include "utilsKernel/logger.h"
#include "connections/clientKernel.h"
#include "connections/serverKernel.h"
#include "utils/utilsGeneral.h"

#include <commons/collections/list.h>


//Controla la Lista NEW y los manda a la lista READY
void newState();

//Espera la llegade de un proceso a EXIT y lo termina
void exitState();

//Crea un proceso con su PCB
pcb_t* createProcess();

//Agrega un proceso a NEW enviando el Path a memoria
void addPcbToNew(char* path);

//Inicia el planificador a largo plazo
void initLongTermPlanning();

//Destruye el proceso y libera la memoria
void destroyProcess(pcb_t *process);

//Compara el pid para encontrar el que proceso que busca.
bool compare_pid(void *data);

//Finaliza el proceso dependiendo en donde se encuentre.
void killProcess(uint32_t *pid);

//Encuentra el pcb y lo segun el pid y lo retorna.
pcb_t* foundStatePcb(uint32_t pid);


#endif