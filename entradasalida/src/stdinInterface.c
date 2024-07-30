#include "stdinInterface.h"

void executeIOStdinReadAndSendResults()
{   
    executeIOStdinRead();

    t_paramsForStdinInterface *params = (t_paramsForStdinInterface*)interfaceData.currentOperation.params;

    writeToMemory(resultsForStdin.resultsForMemory, params->addressesInfo, params->amountOfPhysicalAddresses);

    // Se espera a recibir confirmación de la memoria de que salió todo bien
    //sem_wait(&semaphoreForStdin);

    sendIOStdinReadResultsToKernel();

    free(resultsForStdin.resultsFromRead);
    resultsForStdin.resultsFromRead = NULL;
    free(resultsForStdin.resultsForMemory);
    resultsForStdin.resultsForMemory = NULL;
    free(params->addressesInfo);

    interfaceData.currentOperation.operation = IO_NULL;
    //free(interfaceData.currentOperation.params);
    interfaceData.currentOperation.pid = -1;
}

void executeIOStdinRead()
{   
    log_info(getLogger(), "PID: %d - Operacion: IO_STDIN_READ", (int)interfaceData.currentOperation.pid);

    resultsForStdin.resultsFromRead = readline("> ");

    t_paramsForStdinInterface *params = (t_paramsForStdinInterface*)interfaceData.currentOperation.params;
    memcpy(resultsForStdin.resultsForMemory, resultsForStdin.resultsFromRead, params->totalSize);
}