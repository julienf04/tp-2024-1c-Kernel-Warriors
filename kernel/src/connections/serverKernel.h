#ifndef KERNEL_SERVER_H_
#define KERNEL_SERVER_H_

#include <commons/collections/list.h>
#include <semaphore.h>
#include "utils/server/utils.h"
#include "utilsKernel/logger.h"
#include "utilsKernel/config.h"
#include <pthread.h>
#include <semaphore.h>
#include "../kernel.h"
#include "generalPlanning.h"


#define MAX_IO_CLIENTS 4096


extern sem_t semaphoreForIO;

extern bool _finishAllServersSignal;

void finishAllServersSignal();


// Espera en un loop a los clientes y les crea su hilo para recibir paquetes correspondiente (si es que no se llego al maximo de clientes)
void receiveClientIteration(int socketServer);

/////////////////////////////////////IO///////////////////////////////////////////////////


// Hilo que funciona como servidor para recibir paquetes de un cliente especificado por parametro, y realizar determinadas operaciones en base al paquete recibido
void serverKernelForIO(int* socketClient);


// Funcion que se ejecuta cuando un cliente solicita realizar esta operacion.
void operationPackageFromIO(t_list* package);


void ioSendInterface(int *socketClientIO);


void ioSendEndOperation(int *socketClientIO);


void ioInterfaceDisconnect(int *socketClientIO);


//////////////////////////////////////MEMORIA///////////////////////////////////////////


void serverKernelForMemory(int *socketCLient);


void memorySendResponseForNewProcess(int *socketClientMemory);


//////////////////////////////////////CPU///////////////////////////////////////////////


// Hilo que funciona como servidor para recibir paquetes de un cliente especificado por parametro, y realizar determinadas operaciones en base al paquete recibido
void serverKernelForCPU(int *socketClient);


// Recibe la opCode de que el proceso termino y lo manda a PcbExitList.
void cpuSendExitProcess(int *socketClientCPUDispatch);


void cpuSendOutOfMemoryProcess(int *socketClientCPUDispatch);


void cpuSendInterruptKillProcess(int *socketClientCPUDispatch);


// Recibe el contexto del proceso que paso por la interrupcion del Quantum y manda al proceso a PcbReadyList.
void cpuSendInterruptQ(int *socketClientCPUDispatch);


void cpuSendWaitOfProcess(int *socketClientCPUDispatch);


void cpuSendSignalofProcess(int *socketClientCPUDispatch);


void cpuSendRequestForIOStdoutWrite(int *socketClientCPUDispatch);


void cpuSendRequestForIOStdinRead(int *socketClientCPUDispatch);


void cpuSendRequestForIOGenSleep(int *socketClientCPUDispatch);


void cpuSendRequestForIODialFsCreate(int *socketClientCPUDispatch);


void cpuSendRequestForIODialFsDelete(int *socketCLientCPUDispatch);


void cpuSendRequestForIODialFsTruncate(int *socketClientCPUDispatch);


void cpuSendRequestForIODialFsRead(int *socketClientCPUDispatch);


void cpuSendRequestForIODialFsWrite(int *socketClientCPUDispatch);


// Recibe el contexto del proceso de un package y lo retorna. 
contextProcess recieveContextFromPackage(t_list* package);


pcb_t* assignContextToPcb(contextProcess contextProcess);



#endif