#ifndef CPU_SERVER_H_
#define CPU_SERVER_H_

#include "pthread.h"
#include <semaphore.h>
#include <stdbool.h>
#include <commons/collections/list.h>

#define MAX_KERNEL_CLIENTS_FOR_DISPATCH 1
#define MAX_KERNEL_CLIENTS_FOR_INTERRUPT 1


extern sem_t semaphoreForKernelDispatch;
extern sem_t semaphoreForKernelInterrupt;

extern bool _finishAllServersSignal;

extern int socketMemory;
extern int socketKernelDispatch;
extern int socketKernelInterrupt;


// Manda el aviso a todos los servidores de que deben dejar de recibir peticiones y terminar su ejecucion cuando puedan
void finishAllServersSignal();


// Hilo que funciona como servidor para recibir paquetes de los clientes y realizar determinadas operaciones
void serverCPUDispatchForKernel(int *socketClient);


// Hilo que funciona como servidor para recibir paquetes de los clientes y realizar determinadas operaciones
void serverCPUInterruptForKernel(int *socketClient);


// Hilo que funciona como servidor para recibir paquetes de los clientes y realizar determinadas operaciones
void serverCPUForMemory(int *socketClient);


// Espera en un loop a los clientes para Dispatch y les crea su hilo para recibir paquetes correspondiente (si es que no se llego al maximo de clientes)
void receiveClientIterationDispatch(int socketServer);

// Espera en un loop a los clientes para Interrupt y les crea su hilo para recibir paquetes correspondiente (si es que no se llego al maximo de clientes)
void receiveClientIterationInterrupt(int socketServer);






/////////////////// FUNCIONES AUXILIARES ///////////////////


// Funcion que se ejecuta cuando un cliente solicita realizar esta operacion.
void operationPackageFromKernel(t_list* package);


/// @brief Cuando recibo la siguiente instruccion a ejecutar por parte de la memoria, esta funcion es llamada
/// @param socketClient El socket del cliente de quién recibió la instruccion
void memoryNextInstruction(int* socketClient);


// Recibe el tamaño de pagina por parte de la Memoria.
void memoryTamPaginaRecive(int* socketClient);


// Recibe un frame.
void memoryGetFrame(int* socketClient);


// Recibe una interrupcion de fin de quantum del Kernel
void kernelInterruptEndQuantum(int* socketClient);


// Recibe una interrupcion de fin de proceso por el Kernel
void kernelInterruptEndProcess(int* socketClient);


/// @brief Recibe un contexto (tipicamente del Kernel) y lo asigna al contexto oficial de la CPU
/// @param socketClient El socket del cliente
void receiveContext(int* socketClient);


/// @brief Recibe una informacion data de lectura de memoria
/// @param socketClient El socket del cliente
void memoryReceiveData(int* socketClient);


/// @brief Recibe una confirmacion de escritura por parte de la memoria
/// @param socketClient El socket del cliente
void memoryReceiveConfirmationForWrite(int* socketClient);


/// @brief Recibe una confirmacion de que se hizo correctamente el resize.
/// @param socketClient El socket del cliente.
void memoryReceiveConfirmationForResize(int* socketClient);

/// @brief Recibe un OUT OF MEMORY de la memoria
/// @param socketClient El socket del cliente
void memoryReceiveOutOfMemory(int* socketClient);

#endif