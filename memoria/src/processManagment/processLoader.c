#include "processLoader.h"
#include "utils/utilsGeneral.h"
#include "paging/memoryUser.h"
#include "utilsMemory/delay.h"
#include "utils/mathMemory.h"
#include "utilsMemory/logger.h"
#include "connections/clientMemory.h"
#include "connections/serverMemory.h"



void initProcessesList()
{
    processesList = initListMutex();
}


bool loadProcessByPath(int PID, char* pseudocodePath)
{
    FILE* pseudocodeFile = fopen(pseudocodePath, "r");

    if (pseudocodeFile == NULL)
    {
        return false;
    }


    loadProcessByFile(PID, pseudocodeFile);

    fclose(pseudocodeFile);

    return true;
}



void loadProcessByFile(int PID, FILE* pseudocodeFile)
{
    processInfo* newProcess = malloc(sizeof(processInfo)); // No olvidarse de liberar la memoria antes de eliminar el nodo de la lista.
    newProcess->PID = PID;
    newProcess->pageTable = NULL;

    // Obtener las instrucciones del archivo y ponerlas en el newProcess->pseudocodeInstructions.

    char* buffer = 0;
    long length;

    // Obtengo todo el contenido del archivo de pseudocodigo y lo pongo en un buffer.
    fseek(pseudocodeFile, 0, SEEK_END);
    length = ftell(pseudocodeFile);
    fseek(pseudocodeFile, 0, SEEK_SET);

    buffer = malloc(length + 1);
    
    fread(buffer, 1, length, pseudocodeFile);
    buffer[length] = '\0';


    // Separo las instrucciones del buffer y lo asigno a la informacion de la estructura del pseudocodigo.
    newProcess->pseudocodeInstructions = string_split(buffer, "\n");

    // Libero la memoria del buffer.
    free(buffer);

    newProcess->pageTable = NULL;
    newProcess->amountOfPages = 0;
    newProcess->internalFragmentation = 0;

    // Agrego la informacion del pseudocodigo del nuevo proceso en la lista.
    list_push(processesList, newProcess); 
}


void loadProcessByPathWithParams(void* params)
{
    // Retardo de la operacion
    memoryDelay();


    // Convierto los parametros para tenerlo como un struct
    kernelPathProcess* processPath = (kernelPathProcess*)params;

    // Obtengo el fullPath, que seria la ruta del config + la ruta que me mandó el Kernel
    char* fullPath = string_duplicate(getMemoryConfig()->PATH_INSTRUCCIONES);
    string_append(&fullPath, processPath->path);

    // Cargo el proceso. Me retorna si fue posible cargarlo (true) o no (false)
    bool result = loadProcessByPath(processPath->pid, fullPath);
    

    if (result)
    {
        logCreateProcess(processPath->pid, 0);
    }
    else
    {
        logCreateProcessError(fullPath);
    }

    sendProcessCreatedResult(socketKernel, result);

    free(fullPath);
    free(processPath->path);
    free(processPath);
}


void destroyProcess(int PID)
{
    sem_wait(&semAuxPID);
    auxPID = PID;
    processInfo* info = list_remove_by_condition_mutex(processesList, closurePIDsAreEqual);
    sem_post(&semAuxPID);

    // LIbero las instrucciones guardadas
    string_array_destroy(info->pseudocodeInstructions);


    // Cantidad de bytes a liberar de la memoria de usuario.
    int bytesToFree = getAmountOfBytesAllocated(getMemoryConfig()->TAM_PAGINA, info->amountOfPages, info->internalFragmentation);

    // Necesito un puntero a la tabla de paginas, porque necesito pasarlo como referencia y dentro se va a cambiar la direccion de memoria de la tabla de paginas, y necesito retornar ese valor.
    int** pointerToPageTable = malloc(sizeof(int**));
    *pointerToPageTable = info->pageTable;

    // Libero la memoria de usuario
    int amountOfPagesFree = freeMemory(bytesToFree, pointerToPageTable, &info->amountOfPages, &info->internalFragmentation);
    info->pageTable = *pointerToPageTable;

    free(pointerToPageTable);
    free(info);

    logDestroyProcess(PID, amountOfPagesFree);
}


void destroyProcessWithParams(void* params)
{
    // Retardo de la operacion
    memoryDelay();

    // La operacion
    
    kernelEndProcess* processEnd = params;

    destroyProcess(processEnd->pid);

    free(processEnd);
}