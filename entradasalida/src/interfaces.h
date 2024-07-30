#ifndef IO_INTERFACES_H
#define IO_INTERFACES_H

#include "utilsIO/config.h"
#include "utilsIO/logger.h"
#include "utils/client/utils.h"
#include "utils/server/utils.h"
#include "connections/serverIO.h"
#include "connections/clientIO.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <semaphore.h>
#include <readline/readline.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dirent.h>
typedef enum
{   
    IO_NULL,
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ
} supported_operations;

/*typedef enum
{
    GENERIC_TYPE,
    STDIN_TYPE,
    STDOUT_TYPE,
    DIALFS_TYPE
} interface_type;*/

typedef struct
{   
    uint32_t pid;
    supported_operations operation;
    void* params;
} t_currentOperation;

typedef struct
{
    char *name;
    interfaceType type;
    t_currentOperation currentOperation;
    uint32_t workUnits;
} t_interfaceData;

typedef struct
{
    uint32_t workUnits;
} t_paramsForGenericInterface;

typedef struct
{
    uint32_t amountOfPhysicalAddresses;
    physicalAddressInfo* addressesInfo;
    uint32_t totalSize;
} t_paramsForStdinInterface;

typedef struct
{
    uint32_t amountOfPhysicalAddresses;
    physicalAddressInfo* addressesInfo;
    uint32_t totalSize;
} t_paramsForStdoutInterface;

typedef struct
{
    char *fileName;
} t_paramsForIOFSCreateOrDelete;

typedef struct
{
    char *fileName;
    uint32_t size;
} t_paramsForIOFSTruncate;

typedef struct
{
    char *fileName;
    uint32_t amountOfPhysicalAddresses;
    physicalAddressInfo* addressesInfo;
    uint32_t totalSize;
    uint32_t filePointer;
} t_paramsForIOFSWriteOrRead;

typedef struct
{
    char *resultsFromRead;
    char *resultsForMemory;
} t_resultsForStdin;

typedef struct
{
    char *resultsForWrite;
} t_resultsForStdout;

typedef struct
{
    char *resultsForWrite;
} t_resultsForIOFSWrite;

typedef struct
{
    char *resultsFromRead;
} t_resultsForIOFSRead;

typedef struct
{   
    t_config* metaData; // La metadata del archivo.
    int firstBlockIndex; // El indice del primer bloque del archivo en bloques.dat
    int filePointer; // Puntero del archivo. Es la cantidad de bytes que deben ser desplazados a partir del inicio del archivo para realizar una lectura o escritura.
    int amountOfBlocks; // Cantidad de bloques que tiene asignados el archivo. Es igual a ceil( tamaño_archivo / tamaño_bloque )
    int size; // Cantidad de bytes que ocupa el archivo en bloques.dat
} t_fileData;

typedef struct
{
    FILE* file; // Archivo original de bloques.dat
    void* mappedFile; // Una copia del archivo de bloques en la Memoria. Cuando se modifique algun byte, se deberia llamar a la funcion msync, para que el archivo original tambien se modifique.
    int size; // Cantidad de bytes que ocupa el archivo de bloques.
} t_blocksData;

typedef struct
{
    FILE* file; // Archivo original de bitmap.dat
    void* mappedFile; // Una copia del archivo de bitmap en la Memoria. Cuando se modifique algun byte, se deberia llamar a la funcion msync, para que el archivo original tambien se modifique.
    t_bitarray* bitmap; // bitmap que contiene qué bloques están libres/ocupados. Usa la misma direccion de memoria que mappedFile para fijarse eso, así que siempre que se modifique algun byte, se deberia llamar a la funcion msync, para que el archivo original tambien se modifique.
    int size; // Cantidad de bytes que ocupa el archivo de bitmap.
} t_bitmapData;

typedef struct
{
    t_blocksData blocks; // Data de los bloques del FS
    t_bitmapData bitmap; // Data del bitmap de bloques libres/ocupados del FS.
    t_dictionary* files; // Diccionario que contiene la data de los archivos que están en el FS. Cada entrada del diccionario es un struct t_fileData.
} t_FSData;



extern t_interfaceData interfaceData;
extern t_resultsForStdin resultsForStdin;
extern t_resultsForStdout resultsForStdout;
extern t_resultsForIOFSWrite resultsForIOFSWrite;
extern t_resultsForIOFSRead resultsForIOFSRead;

extern void* dataReceivedFromMemory;

extern sem_t semaphoreSendDataToMemory;
extern sem_t semaphoreReceiveDataFromMemory;
extern sem_t semaphoreForModule;

extern int socketKernel;
extern int socketMemory;

extern t_FSData fsData;


void createInterface(char* name);

void destroyInterface();

void* openCreateMapFile(FILE** file, char* fileName, int fileSize);

void createDictionaryFileNames();

void closeBlocksFile();

void closeBitmapFile();

void closeAllFiles();




/////////////////////////////// FUNCIONES AUXILIARES ///////////////////////////////



/// @brief Le manda la data especificada a la Memoria, junto con las direcciones fisicas.
/// @param data La data a mandar.
/// @param addressesInfo Array de physicalAddressInfo, donde cada indice contiene la direccion fisica y el tamaño a escribir en la misma.
/// @param amountOfPhysicalAddresses La cantidad de direcciones fisicas (el tamaño del array).
void writeToMemory(void* data, physicalAddressInfo* addressesInfo, int amountOfPhysicalAddresses);

/// @brief Le pide a Memoria la data que está en la direccion fisica dada, con su tamaño.
/// @param addressesInfo Array de physicalAddressInfo, donde cada indice contiene la direccion fisica y el tamaño a escribir en la misma.
/// @param amountOfPhysicalAddresses La cantidad de direcciones fisicas (el tamaño del array).
/// @param size Tamaño total a leer.
/// @return Retorna la data leida.
void* readFromMemory(physicalAddressInfo* addressesInfo, int amountOfPhysicalAddresses, int size);


/// @brief Retorna el path + fileName, siendo el path el indicado por el atributo PATH_BASE_DIALFS del archivo de configuracion de la IO.
/// @param fileName El nombre del archivo, sin el path previo.
/// @return Retorna el nombre completo del archivo (path + fileName)
/// @warning La funcion llama a malloc para reservar memoria para el string retornado. Se debe liberar luego con free
char* getFullFileName(char* fileName);


/// @brief Retorna el primer byte del bloque especificado
/// @param blockIndex El indice del bloque
int getFirstByteOfBlock(int blockIndex);


/// @brief Obtiene la cantidad de bloques que tiene reservados un archivo.
/// @param fileSize El tamaño del archivo.
/// @return Retorna la cantidad de bloques.
int getAmountOfBlocks(int fileSize);


/// @brief Realiza el delay de la compactacion.
void delayCompacting();


/// @brief Realiza el delay correspondiente para cualquier operacion del FS.
void delayFS();



#endif