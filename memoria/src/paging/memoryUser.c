#include "memoryUser.h"


#include <stdio.h>
#include <stdlib.h>
#include "utilsMemory/config.h"
#include "math.h"
#include "utilsMemory/delay.h"
#include "processManagment/essentials.h"
#include <commons/log.h>
#include "utilsMemory/logger.h"
#include "utils/mathMemory.h"
#include <string.h>



// El espacio contiguo de memoria que representa la memoria del usuario
void* memoryUser;


// Array de bools que dicen si un frame está libre (false) u ocupado (true)
bool* memoryUserFrames;


sem_t semMemoryUserFrames;


// Cantidad maxima de frames que tiene la memoria de usuario
int maxFrames;


// Cantidad de frames libres que tiene la memoria de usuario actualmente.
int amountOfFramesFree;


// Ultimo framq que se comprobó si está libre u ocupado.
int lastIndexChecked;




void initMemoryUser()
{
    memoryUser = malloc(getMemoryConfig()->TAM_MEMORIA);

    maxFrames = getMemoryConfig()->TAM_MEMORIA / getMemoryConfig()->TAM_PAGINA;

    memoryUserFrames = calloc(maxFrames, sizeof(bool)); // calloc para que todas las paginas se inicialicen como libres (en 0, false)

    sem_init(&semMemoryUserFrames, 0, 1);

    amountOfFramesFree = maxFrames;

    lastIndexChecked = -1;
}


void* getMemoryUser()
{
    return memoryUser;
}


bool isFrameBusy(int frame)
{
    return memoryUserFrames[frame];
}



int pidAux; // Pid auxiliar usado para la closure para buscar el pid en una lista

bool closureFindPID(processInfo* processInfo)
{
    return pidAux == processInfo->PID;
}


allocationResult resizeMemory(int pid, int bytes)
{
    memoryDelay();

    allocationResult result = RESIZE_SUCCESS;

    sem_wait(&semAuxPID);
    pidAux = pid;
    processInfo* info = (processInfo*)list_find_mutex(processesList, closureFindPID);
    sem_post(&semAuxPID);

    // Necesito un puntero a la tabla de paginas, porque necesito pasarlo como referencia y dentro se va a cambiar la direccion de memoria de la tabla de paginas, y necesito retornar ese valor.
    int** pointerToPageTable = malloc(sizeof(int**));
    *pointerToPageTable = info->pageTable;


    int amountOfBytesAllocated = getAmountOfBytesAllocated(getMemoryConfig()->TAM_PAGINA, info->amountOfPages, info->internalFragmentation);

    if (bytes > amountOfBytesAllocated) // Hay que reservar más espacio en memoria del usuario
    {
        logProcessSizeExpansion(pid, amountOfBytesAllocated, bytes - amountOfBytesAllocated);

        allocMemory(bytes - amountOfBytesAllocated, pointerToPageTable, &info->amountOfPages, &info->internalFragmentation, &result);
        info->pageTable = *pointerToPageTable;
    }
    else if (bytes < amountOfBytesAllocated) // Hay que liberar espacio en memoria del usuario
    {
        logProcessSizeReduction(pid, amountOfBytesAllocated, amountOfBytesAllocated - bytes);

        freeMemory(amountOfBytesAllocated - bytes, pointerToPageTable, &info->amountOfPages, &info->internalFragmentation);
        info->pageTable = *pointerToPageTable;
    }
    else // Entra en este else cuando bytes == amountOfBytesAllocated
    {
        logProcessSizeNotChange(pid, amountOfBytesAllocated);
    }

    free(pointerToPageTable);

    return result;
}


int allocMemory(int bytes, int** pages, int* const amountOfPages, int* const internalFragmentation, allocationResult* result)
{
    ///////////// CASOS DE CORTE /////////////


    // Compruebo si los bytes que se quieren reservar entran en la fragmentacion interna que sobró de la ultima pagina actual.
    // Si es asi, uso esa memoria de fragmentacion interna como memoria de usuario valida.
    // Tambien entra en este if si se intentan reservar 0 bytes, caso en el que no cambiaria nada y solamente retornaria 0
    if (bytes <= *internalFragmentation)
    {
        *internalFragmentation -= bytes;
        return 0;
    }


    // Lleno la ultima pagina actual que tenia fragmentacion interna, para que todas las paginas actuales queden ocupadas completamente.
    bytes -= *internalFragmentation;
    

    // Obtengo la cantidad de paginas que deberian reservarse.
    int amountOfNewPages = getAmountOfPagesAllocated(getMemoryConfig()->TAM_PAGINA, bytes);


    /////// A partir de aca solo un hilo puede reservar o liberar la memoria, para que no haya inconsistencias poniendo como libres u ocupados los frames ///////
    sem_wait(&semMemoryUserFrames);



    // Si no hay suficientes paginas para reservar la cantidad que se pide, retorna Out Of Memory (-1)
    if (amountOfNewPages > amountOfFramesFree)
    {
        sem_post(&semMemoryUserFrames);
        *result = OUT_OF_MEMORY;
        return OUT_OF_MEMORY;
    }



    ///////////// ALOJO SEGURO A PARTIR DE ACA, AL MENOS SE RESERVA UNA NUEVA PAGINA /////////////



    // Obtengo el indice desde el que hay que empezar a buscarle frames libres a las paginas.
    int newPagesIndex = *amountOfPages;

    // Obtengo la cantidad total de paginas que ocupará todo el proceso.
    *amountOfPages +=  amountOfNewPages;


    // Expando el array de paginas
    *pages = realloc(*pages, *amountOfPages * sizeof(int));

    // Le asigno un frame libre a cada nueva pagina.
    for ( ; newPagesIndex < *amountOfPages; newPagesIndex++)
    {
        (*pages)[newPagesIndex] = allocNextFrameFree();
    }


    // A partir de aca, ya se terminó de settear los valores correctos a los frames de la memoria.
    sem_post(&semMemoryUserFrames);


    // Obtengo la nueva fragmentacion interna de la ultima pagina
    *internalFragmentation = getInternalFragmentation(getMemoryConfig()->TAM_PAGINA, bytes);


    *result = RESIZE_SUCCESS;
    return amountOfNewPages;
}



int freeMemory(int bytes, int** pages, int* const amountOfPages, int* const internalFragmentation)
{
    ///////////// CASOS DE CORTE /////////////


    // Si se intenta liberar 0 bytes, no hace nada y retorna que se liberaron 0 paginas.
    // Teoricamente, este caso solo es necesario para contemplar la posibilidad de que la memoria del proceso sea 0 y se intentasen liberar 0 bytes
    if (bytes <= 0)
    {
        return 0;
    }

    // Obtengo la cantidad de bytes que hay reservados en la ultima pagina.
    int amountOfBytesInTheLastPage = getAmountOfBytesInTheLastPage(getMemoryConfig()->TAM_PAGINA, *internalFragmentation);

    // Compruebo si la cantidad de bytes que quiero liberar es posible liberarlos solamente de la ultima pagina.
    // Si es asi, los libero de la ultima pagina solamente.
    // Tambien entra en este if si se intentan liberar 0 bytes, caso en el que no cambiaria nada y solamente retornaria 0.
    if (bytes < amountOfBytesInTheLastPage)
    {
        *internalFragmentation += bytes;
        return 0;
    }



    ///////////// LIBERACION SEGURA A PARTIR DE ACA, AL MENOS SE LIBERA UNA PAGINA /////////////


    // Libero completa la ultima liberado todos sus bytes reservados.
    bytes -= amountOfBytesInTheLastPage;


    // Obtengo la cantidad de paginas que debo liberar.
    //int amountOfPagesToFree = floor(bytes / getMemoryConfig()->TAM_PAGINA) + 1;
    int amountOfPagesToFree = getAmountOfPagesToFree(getMemoryConfig()->TAM_PAGINA, bytes);


    // Indice con el que recorrer el final del array de paginas.
    int i = *amountOfPages - 1;

    // Actualizo la cantidad de paginas.
    *amountOfPages -= amountOfPagesToFree;



    /////// A partir de aca solo un hilo puede reservar o liberar la memoria, para que no haya inconsistencias poniendo como libres u ocupados los frames ///////
    sem_wait(&semMemoryUserFrames);


    // Libero los frames que ya no estan en uso.
    for ( ; i >= *amountOfPages; i--)
    {
        freeFrame((*pages)[i]);
    }


    // A partir de aca, ya se terminó de settear los valores correctos a los frames de la memoria.
    sem_post(&semMemoryUserFrames);

    
    // Reduzco el array de paginas
    *pages = realloc(*pages, *amountOfPages * sizeof(int));


    // Obtengo la nueva fragmentacion interna de la ultima pagina
    *internalFragmentation = bytes % getMemoryConfig()->TAM_PAGINA;


    return amountOfPagesToFree;
}




int allocNextFrameFree()
{
    do
    {
        lastIndexChecked = (lastIndexChecked + 1) % maxFrames;
    } while (isFrameBusy(lastIndexChecked));
    
    memoryUserFrames[lastIndexChecked] = true;

    amountOfFramesFree--;

    return lastIndexChecked;
}



void freeFrame(int frame)
{
    memoryUserFrames[frame] = false;

    amountOfFramesFree++;
}





void* readBytes(int pid, int physicalAddress, int size)
{
    memoryDelay();

    void* data = malloc(size);
    
    memcpy(data, memoryUser + physicalAddress, size);

    logReadBytes(pid, physicalAddress, size);

    return data;
}


void writeBytes(int pid, void* data, int physicalAddress, int size)
{
    memoryDelay();

    memcpy(memoryUser + physicalAddress, data, size);

    logWriteBytes(pid, physicalAddress, size);
}


int getFrame(int PID, int page)
{
    memoryDelay();

    processInfo* info;
    sem_wait(&semAuxPID);
    auxPID = PID;
    info = list_find_mutex(processesList, closurePIDsAreEqual);
    sem_post(&semAuxPID);

    logPageTableAccess(PID, page, info->pageTable[page]);

    return info->pageTable[page];
}