
#ifndef GENERAL_PLANNING_H
#define GENERAL_PLANNING_H

#include <commons/collections/list.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include "utils/utilsGeneral.h"
#include "utils/listMutex.h"
#include <commons/temporal.h>

//Estructura de los estados del PCB. 
typedef enum
{
	PCB_NEW,
	PCB_READY,
	PCB_READY_PLUS,
	PCB_EXEC,
	PCB_BLOCK,
	PCB_EXIT
} pcbState_t;

typedef enum
{
	FS_CREATE,
	FS_DELETE,
	FS_TRUNCATE,
	FS_READ,
	FS_WRITE,
} typeOfOperationFS;

typedef struct
{
	uint32_t param1;
	uint32_t param2;
	char* param3;
	uint32_t param4;
	typeOfOperationFS typeOpFs;
	bool isWaitingFs;
	t_list* listAux;
} paramsKernelForIO;

//Estructra del pcb del proceso
typedef struct 
{
	uint32_t pid;
	uint32_t pc;
	t_temporal* quantumForVRR;
	paramsKernelForIO *params;
	t_registers *registersCpu;
	listMutex_t *resources;
	pcbState_t state;
	bool isInInterface;
	bool processKilled;
} pcb_t;

// Estructura de un recurso en Kernel. 
typedef struct 
{
	char* name;
	int instances;
	pthread_mutex_t mutexForInstances;
	listMutex_t *blockList;
} resource_t;

// Estructura de una interfaz en Kernel.
typedef struct 
{
	bool flagKillProcess;
	char* name;
	interfaceType interfaceType;
	bool isBusy;
	pcb_t *processAssign;
	int *socket;
	listMutex_t *blockList;
} interface_t;

// Tipo de algoritmo seleccionado. 
typedef enum
{
	FIFO,
	RR,
	VRR,
} t_algorithm;

// Parametros para el hilo de quantum para VRR.
typedef struct{
	pcb_t* process;
	int64_t timeForQuantum;
} paramsQuantumVRRThread;


extern listMutex_t *pcbNewList;
extern listMutex_t *pcbReadyList;
extern listMutex_t *pcbExecList;
extern listMutex_t *pcbBlockList;
extern listMutex_t *pcbExitList;

extern listMutex_t *pcbReadyPriorityList;

extern listMutex_t *resourcesBlockList;

extern listMutex_t *interfacesList;

extern sem_t semNew;
extern sem_t semExit;
extern sem_t semReady;
extern sem_t semBlock;
extern sem_t semExec;

extern sem_t semMultiProgramming;
extern sem_t semMultiProcessing;
extern sem_t semAddPid;

extern sem_t semMemoryOk;

extern sem_t semPausePlanning;

extern sem_t semKillProcessInInterface;

extern sem_t semKillProcessExec;

extern pthread_mutex_t mutexSendProcessToMemory;

extern pthread_mutex_t mutexOrderProcessByScript;

extern pthread_mutex_t mutexOrderReadyExecProcess;

extern pthread_mutex_t mutexOrderPcbReadyPlus;

extern bool flagMemoryResponse;

extern bool flagExecutingScript;

extern int pid;

extern int socketClientMemory;
extern int socketClientCPUDispatch;
extern int socketClientCPUInterrupt;

extern t_algorithm algorithm;

extern int diffBetweenNewAndPrevMultiprogramming;

#endif
