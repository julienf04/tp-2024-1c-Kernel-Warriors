#include <stdlib.h>
#include <stdio.h>
#include "utilsMemory/logger.h"
#include "utilsMemory/config.h"
#include "connections/serverMemory.h"
#include "utils/server/utils.h"
#include <pthread.h>
#include "finish.h"
#include "processManagment/processLoader.h"
#include "paging/memoryUser.h"
#include "memoryDebug.h"
#include "utilsMemory/debugUtils.h"
#include "utils/config.h"



int main(int argc, char *argv[])
{
    // Inicio el logger general del modulo. Siempre deberia ser la primera sentencia a ejecutar del main.
    initLogger("memoria.log", "memoria", true, LOG_LEVEL_INFO);

    // Obtengo la configuracion general.
    #ifdef DEBUG_MEMORY
    initMemoryConfig("memoria.config");
    #else
    char* path = askForConfigPath();
    initMemoryConfig(path);
    free(path);
    #endif



    #ifdef DEBUG_MEMORY

    logPreInitialMessageDebug();

    #endif


    // Reservo memoria para los semaforos y lo inicializo
    sem_init(&semaphoreForKernel, 0, 1);
    sem_init(&semaphoreForCPU, 0, 1);
    sem_init(&semaphoreForIO, 0, 1);
    sem_init(&semaphoreFinishModule, 0, 0);
    sem_init(&semAuxPID, 0, 1);
    sem_init(&semMemoryUserFrames, 0, 1);


    initMemoryUser();

    initProcessesList();


    // Creo y pongo a correr el/los threads de el/los servidores de este modulo
    waitClientsLoopParams params;
    params.logger = getLogger();
    params.portToListen = getMemoryConfig()->PUERTO_ESCUCHA;
    params.eachIterationFunc = receiveClientIteration;
    params.finishLoopSignal = &_finishAllServersSignal;
    pthread_t waitClientsLoopThread;
    pthread_create(&waitClientsLoopThread, NULL, (void*)waitClientsLoop, &params);
    pthread_detach(waitClientsLoopThread);




    #ifdef DEBUG_MEMORY

    pthread_t printRegistersThreadVar;
    pthread_create(&printRegistersThreadVar, NULL, (void*)printMemoryThread, NULL);
    pthread_join(printRegistersThreadVar, NULL);

    logInitialMessageDebug();

    #else

    logInitialMessageRealese();

    #endif



    // Espero a que me manden la señal de que tengo que terminar finalizar este modulo
    sem_wait(&semaphoreFinishModule);

    // Lanzando la senial a los servidores de que no deben escuchar mas clientes ni realizar ninguna operacion
    finishAllServersSignal();

    // Liberando todos los recursos
    freeMemoryConfig();
    destroyLogger();

    sem_destroy(&semaphoreForKernel);
    sem_destroy(&semaphoreForCPU);
    sem_destroy(&semaphoreForIO);
    sem_destroy(&semaphoreFinishModule);

    return 0;
}
