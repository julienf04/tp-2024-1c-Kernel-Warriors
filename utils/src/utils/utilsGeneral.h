#ifndef UTILS_GENERAL_H_
#define UTILS_GENERAL_H_

#include <commons/log.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <commons/string.h>
#include <semaphore.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <netdb.h>
#include <commons/temporal.h>
#include "listMutex.h"
#include "utils/debugMode.h"


#define DEFAULT_CASE_MESSAGE "Un cliente me envió una operación no válida o desconocida."
#define ERROR_CASE_MESSAGE "Un cliente se desconectó."

// Estructura usada para saber de que tipo es el paquete que envia un cliente.
typedef enum
{
    ERROR = -1,
	DO_NOTHING = 0, // Segun testeos, cuando un cliente se conecta, en su primer o segundo mensaje, envia un mensaje vacio (es decir, con codigo operacion 0). No es un error, simplemente es algo a tener en cuenta y no hacer nada al respecto.
	EXIT_CPU,
	EXIT_MEMORY,

	// HANDSHAKES
	KERNEL_MODULE,
	KERNEL_MODULE_TO_CPU_DISPATCH,
	KERNEL_MODULE_TO_CPU_INTERRUPT,
	CPU_MODULE,
	MEMORY_MODULE,
	IO_MODULE,

	// MENSAJES GENERICOS (PUEDE ENVIARLO MÁS DE UN MODULO)
	READ_MEMORY,
	WRITE_MEMORY,

	// ENVIADOS DESDE CPU
	CPU_GIVE_ME_NEXT_INSTRUCTION,
	CPU_GET_FRAME,
	CPU_RESIZE_MEMORY,
	CPU_SEND_CONTEXT_FOR_EXIT,
	CPU_SEND_CONTEXT_FOR_END_QUANTUM,
	CPU_SEND_CONTEXT_FOR_END_PROCESS,
	CPU_SEND_CONTEXT_FOR_OUT_OF_MEMORY,
	CPU_SEND_CONTEXT_FOR_WAIT,
	CPU_SEND_CONTEXT_FOR_SIGNAL,
	CPU_SEND_CONTEXT_FOR_IO_GENERIC,
	CPU_SEND_CONTEXT_FOR_IO_STDIN,
	CPU_SEND_CONTEXT_FOR_IO_STDOUT,
	CPU_SEND_CONTEXT_FOR_IO_FS_CREATE,
	CPU_SEND_CONTEXT_FOR_IO_FS_DELETE,
	CPU_SEND_CONTEXT_FOR_IO_FS_TRUNCATE,
	CPU_SEND_CONTEXT_FOR_IO_FS_WRITE,
	CPU_SEND_CONTEXT_FOR_IO_FS_READ,

	// ENVIADOS DESDE MEMORIA
	MEMORY_NEXT_INSTRUCTION,
	MEMORY_SEND_DATA,
	MEMORY_WRITE_OK,
	MEMORY_RESIZE_OK,
	MEMORY_OUT_OF_MEMORY,
	MEMORY_SEND_FRAME,
	MEMORY_TAM_PAGINA,
	MEMORY_SEND_RESPONSE_FOR_NEW_PROCESS,

	// ENVIADOS DESDE KERNEL
	KERNEL_SEND_PROCESS_PATH,
	KERNEL_END_PROCESS,

	KERNEL_SEND_CONTEXT,
	KERNEL_SEND_INTERRUPT_QUANTUM_END,
	KERNEL_SEND_INTERRUPT_CONSOLE_END_PROCESS,
	
	KERNEL_SEND_OPERATION_TO_GENERIC_INTERFACE,
	KERNEL_SEND_OPERATION_TO_STDIN_INTERFACE,
	KERNEL_SEND_OPERATION_TO_STDOUT_INTERFACE,
	KERNEL_SEND_OPERATION_FOR_IO_FS_CREATE,
	KERNEL_SEND_OPERATION_FOR_IO_FS_DELETE,
	KERNEL_SEND_OPERATION_FOR_IO_FS_TRUNCATE,
	KERNEL_SEND_OPERATION_FOR_IO_FS_WRITE,
	KERNEL_SEND_OPERATION_FOR_IO_FS_READ,

	// ENVIADOS DESDE IO
	IO_SEND_INTERFACE,
	IO_OK,

	// TODOS LOS SIGUIENTES VALORES SON UNICAMENTE PARA TESTEOS
	PACKAGE_FROM_KERNEL,
	PACKAGE_FROM_CPU,
	PACKAGE_FROM_MEMORY,
	PACKAGE_FROM_IO,
} operationCode;

typedef struct
{
	uint8_t AX; // Registro Numérico de propósito general
	uint8_t BX; // Registro Numérico de propósito general
	uint8_t CX; // Registro Numérico de propósito general
	uint8_t DX; // Registro Numérico de propósito general
	uint32_t EAX; // Registro Numérico de propósito general
	uint32_t EBX; // Registro Numérico de propósito general
	uint32_t ECX; // Registro Numérico de propósito general
	uint32_t EDX; // Registro Numérico de propósito general
	uint32_t SI; // Contiene la dirección lógica de memoria de origen desde donde se va a copiar un string.
	uint32_t DI; // Contiene la dirección lógica de memoria de destino a donde se va a copiar un string.
} t_registers;

// Tipos de interfaces. 
typedef enum
{
	Generic, 
	STDIN, 
	STDOUT, 
	DialFS
} interfaceType;

//Estructura del contexto de ejecucion de los procesos
typedef struct
{
	uint32_t pc;
	t_registers registersCpu;
} contextProcess;

// Estructura que se le manda a la memoria del path y el pid del proceso a ejecutar.
typedef struct
{
	uint32_t pid;
	char* path;
} kernelPathProcess;

// Estructura que se le manda a la memoria para que elimine un proceso.
typedef struct
{
	uint32_t pid;
} kernelEndProcess;

// Estructura que se le manda a la memoria para decirle que debe enviarle a la CPU la proxima instruccion.
typedef struct
{
	int PID;
	uint32_t PC; // Program Counter
} cpuGiveMeNextInstruction;


// Estructura que se le manda a la CPU con el string de la instruccion que debe ejecutar
typedef struct
{
	char* string;
} instructionString;


typedef struct
{
	int pid;
	int bytes;
} requestResizeMemoryInfo;


typedef struct
{
	int pid;
	int physicalAddress;
	int size;
} requestReadMemoryInfo;


typedef struct
{
	int pid;
	void* data;
	int physicalAddress;
	int size;
} requestWriteMemoryInfo;


typedef struct
{
	void* data;
	int size;
} sendDataInfo;


typedef struct
{
	int PID;
	int page;
} getFrameInfo;



typedef struct
{
	int frame;
} sendFrameInfo;

typedef struct
{
	int tamPagina;
} memoryTamPagina;


typedef struct
{
	int physicalAddress;
	int size;
} physicalAddressInfo;

typedef struct
{
	int* physicalAddress;
	int* size;
} physicalAddressInfoP;













#endif