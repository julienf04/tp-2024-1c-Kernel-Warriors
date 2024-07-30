#ifndef CLIENT_KERNEL
#define CLIENT_KERNEL

#include <commons/collections/list.h>
#include <semaphore.h>
#include "utils/client/utils.h"
#include "utilsKernel/logger.h"
#include "utilsKernel/config.h"
#include <pthread.h>
#include <semaphore.h>
#include "../kernel.h"

//////////// A CPU ///////////////


/// @brief Le envia el contexto del proceso a CpuDispatch para que ejecute.
/// @param process El pcb del proceso
void sendContextToCPU(pcb_t *process);


/// @brief Le envia a CPUInterrupt la interrupcion de fin de Quantum.
void sendInterruptForQuantumEnd(pcb_t *process);


/// @brief Le envia a CPUInterrupt la interrupcion de finalizar proceso por la consola.
void sendInterruptForConsoleEndProcess(pcb_t *process);


//////////// A IO ///////////////


/// @brief Le avisa a la interfaz que realice la operacion de IO_GEN_SLEEP con su tiempo de la operacion.
/// @param interface La interfaz que tiene el nombre identificador y su socket.
/// @param timeOfOperation Tiempo que necesita la operacion.
void sendIOGenSleepOperationToIO(interface_t *interface, uint32_t timeOfOperation);


/// @brief Le avisa a la interfaz que realice la operacion de IO_STDIN_READ con su direccion de registro y registro de tamaño.
/// @param interface La interfaz que tiene el nombre identificador y su socket.
/// @param listOfPhysicalAdressesInfo Lista que contiene todas las direcciones fisicas.
/// @param amountOfPhysicalAddresses Cantidad de direcciones fisicas que contiene la lista.
/// @param sizeToReadOrWrite Suma de tamanio de direcciones fisicas.
void sendIOStdinReadOperationToIO(interface_t* interface, t_list* listOfPhysicalAdressesInfo, int amountOfPhysicalAddresses, int sizeToReadOrWrite);


/// @brief Le avisa a la interfaz que realice la operacion de IO_STDOUT_WRITE con su direccion de registro y registro de tamaño.
/// @param interface La interfaz que tiene el nombre identificador y su socket.
/// @param listOfPhysicalAdressesInfo Lista que contiene todas las direcciones fisicas.
/// @param amountOfPhysicalAddresses Cantidad de direcciones fisicas que contiene la lista.
/// @param sizeToReadOrWrite Suma de tamanio de direcciones fisicas.
void sendIOStdoutWriteOperationToIO(interface_t* interface, t_list* listOfPhysicalAdressesInfo, int amountOfPhysicalAddresses, int sizeToReadOrWrite);


void sendIODialFsCreateOperationToIO(interface_t *interface, char* nameOfFile);


void sendIODialFsDeleteOperationToIO(interface_t *interface, char* nameOfFIle);


void sendIODialFsTruncateOperationToIO(interface_t *interface, char* nameOfFile, uint32_t size);


void sendIODialFsReadOperationToIO(interface_t *interface, char* nameOfFile, t_list* listOfPhysicalAdressesInfo, int amountOfPhysicalAddresses, int sizeToReadOrWrite, int pointer);


void sendIODialFsWriteOperationToIO(interface_t *interface, char* nameOfFile, t_list* listOfPhysicalAdressesInfo, int amountOfPhysicalAddresses, int sizeToReadOrWrite, int pointer);


//////////// A MEMORIA ///////////////


/// @brief Le envia a la memoria el pid del proceso y el path dado por la consola que va a tener 
// la ubicacion de las instrucciones del proceso a ejecutar.
/// @param process El pcb del proceso que contiene su pid.
/// @param path ubicacion del archivo de pseudocodigo con las instrucciones.
void sendProcessPathToMemory(pcb_t *process, char* path);


/// @brief Le envia a la memoria el pid del proceso a finalizar y borrar. 
/// @param process El pcb del proceso que contiene su pid.
void sendEndProcessToMemory(pcb_t *processToEnd);


#endif