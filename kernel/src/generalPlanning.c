#include "generalPlanning.h"

listMutex_t *pcbNewList;
listMutex_t *pcbReadyList;
listMutex_t *pcbExecList;
listMutex_t *pcbBlockList;
listMutex_t *pcbExitList;
listMutex_t *pcbReadyPriorityList;

listMutex_t *resourcesBlockList;

listMutex_t *interfacesList;

sem_t semNew;
sem_t semExit;
sem_t semReady;
sem_t semBlock;
sem_t semExec;

sem_t semMultiProgramming;
sem_t semMultiProcessing;
sem_t semAddPid;

sem_t semMemoryOk;
sem_t semPausePlanning;
sem_t semKillProcessInInterface;
sem_t semKillProcessExec;

pthread_mutex_t mutexSendProcessToMemory;

pthread_mutex_t mutexOrderProcessByScript;

pthread_mutex_t mutexOrderReadyExecProcess;

pthread_mutex_t mutexOrderPcbReadyPlus;

bool flagMemoryResponse;

bool flagExecutingScript;

int pid;

t_algorithm algorithm;

int socketClientMemory;
int socketClientCPUDispatch;
int socketClientCPUInterrupt;

int diffBetweenNewAndPrevMultiprogramming; 
