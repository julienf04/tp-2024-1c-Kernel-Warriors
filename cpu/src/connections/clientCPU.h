#ifndef CPU_CLIENT_H_
#define CPU_CLIENT_H_


#include <stdint.h>
#include "utils/utilsGeneral.h"
#include "utils/client/utils.h"
#include "serverCPU.h"
#include "instructionCycle/essentials.h"



/// @brief Le envia mediante a la memoria el Program Counter pasado por parametro mediante sockets.
/// @param PID El Process ID
/// @param PC El Program Counter
void sendPCToMemory(int PID, uint32_t PC);



/// @brief Le pide a la Memoria el frame correspondiente a los parametros
/// @param pid Process id
/// @param page La pagina para pedir
void requestFrame(int pid, int page);


/// @brief Manda una peticion de resize a la memoria
/// @param pid Process ID
/// @param bytes Los bytes totales de memoria de usuario que deberá resize-earse paratener el procesos
void sendResizeMemory(int pid, int bytes);


/// @brief Manda una peticion de lectura a la memoria
/// @param pid Process ID
/// @param physicalAddress La direccion fisica
/// @param size El tamaño
void sendReadMemory(int pid, int physicalAddress, int size);


/// @brief Manda una peticion de escritura a la memoria
/// @param pid Process ID
/// @param data La data
/// @param physicalAddress La direccion fisica
/// @param size El tamaño
void sendWriteMemory(int pid, void* data, int physicalAddress, int size);


/// @brief Envia el contexto actualizado del proceso al Kernel
/// @param opCode El codigo de operacion por el que se va a enviar el contexto. Es el motivo de enviar el contexto.
void sendContextToKernel(operationCode opCode);


/// @brief Envia el contexto actualizado del proceso al Kernel con un motivo de WAIT o SIGNAL con el recurso resource.
/// @param opCode El codigo de operacion por el que se va a enviar el contexto. Es el motivo de enviar el contexto.
/// @param resource El nombre del recurso.
void sendContextToKernelForResource(operationCode opCode, char* resource);


/// @brief Envia el contexto actualizado del proceso al Kernel con un motivo de IO GENERIC, enviando ademas el nombre de la interfaz y las unidades de trabajo.
/// @param nameInterface El nombre de la interfaz.
/// @param workUnits Las unidades de trabajo.
void sendContextToKernelForIOGeneric(char* nameInterface, uint32_t workUnits);


/// @brief Envia el contexto actualizado del proceso al Kernel con un motivo de IO STDIN READ o IO STDOUT WRITE,
/// enviando ademas el nombre de la interfaz, la cantidad de direcciones fisicas,
/// la info de cada una de las direcciones fisicas en orden (es decir, la direccion fisica en sí misma y luego el tamaño a leer/escribir en esa direccion fisica),
/// y el tamaño total a leer/escribir para la interfaz.
/// @param opCode El codigo de operacion por el que se va a enviar el contexto. Es el motivo de enviar el contexto.
/// @param nameInterface El nombre de la interfaz.
/// @param amountOfPhysicalAddresses La cantidad de direcciones fisicas.
/// @param physicalAddressesArray El array que contiene la info de todas las direcciones fisicas en orden.
/// @param sizeToReadOrWrite El tamaño total a leer/escribir para la interfaz.
void sendContextToKernelForIOReadOrWrite(operationCode opCode, char* nameInterface, int amountOfPhysicalAddresses, physicalAddressInfo* physicalAddressesArray, int sizeToReadOrWrite);


/// @brief Envia el contexto actualizado del proceso al Kernel con un motivo de IO FS CREATE o IO FS DELETE, junto a sus parametros.
/// @param opCode El codigo de operacion (para saber si es IO_FS_CREATE o IO_FS_DELETE)
/// @param nameInterface El nombre de la interfaz.
/// @param fileName El nombre del archivo.
void sendContextToKernelForIOFSCreateOrDelete(operationCode opCode, char* nameInterface, char* fileName);


/// @brief Envia el contexto actualizado del proceso al Kernel con un motivo de IO FS TRUNCATE, junto a sus parametros.
/// @param nameInterface El nombre de la interfaz.
/// @param fileName El nombre del archivo.
/// @param size EL tamaño a truncar
void sendContextToKernelForIOFSTruncate(char* nameInterface, char* fileName, uint32_t size);


/// @brief Envia el contexto actualizado del proceso al Kernel con un motivo de IO FS READ o IO FS WRITE, junto a sus parametros.
/// @param opCode El codigo de operacion (para saber si es IO_FS_READ o IO_FS_WRITE)
/// @param nameInterface El nombre de la interfaz.
/// @param fileName El nombre del archivo.
/// @param amountOfPhysicalAddresses La cantidad de direcciones fisicas.
/// @param physicalAddressesArray El array que contiene la info de todas las direcciones fisicas en orden.
/// @param sizeToReadOrWrite El tamaño total a leer/escribir para la interfaz.
/// @param filePointer El puntero al archivo
void sendContextToKernelForIOFSReadOrWrite(operationCode opCode, char* nameInterface, char* fileName, int amountOfPhysicalAddresses, physicalAddressInfo* physicalAddressesArray, int sizeToReadOrWrite, int filePointer);






/////////////////// FUNCIONES AUXILIARES ///////////////////


void addContextToPackage(t_package* package, contextProcess* context);








#endif