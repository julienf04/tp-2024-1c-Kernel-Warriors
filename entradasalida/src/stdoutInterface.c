#include "stdoutInterface.h"

void executeIOStdoutWriteAndSendResults()
{
    executeIOStdoutWrite();

    t_paramsForStdinInterface *params = (t_paramsForStdinInterface*)interfaceData.currentOperation.params;

    sendIOStdoutWriteResultsToKernel();

    free(resultsForStdout.resultsForWrite);
    resultsForStdout.resultsForWrite = NULL;

    interfaceData.currentOperation.operation = IO_NULL;
    //free(interfaceData.currentOperation.params);
    interfaceData.currentOperation.pid = -1;
    free(params->addressesInfo);
}

void executeIOStdoutWrite()
{   

    log_info(getLogger(), "PID: %d - Operacion: IO_STDOUT_WRITE", (int)interfaceData.currentOperation.pid);

    t_paramsForStdinInterface *params = (t_paramsForStdinInterface*)interfaceData.currentOperation.params;

    resultsForStdout.resultsForWrite = (char*)readFromMemory(params->addressesInfo, params->amountOfPhysicalAddresses, params->totalSize);

    resultsForStdout.resultsForWrite[params->totalSize] = '\0';
    
    //Se espera a recibir el contenido de la memoria
    //sem_wait(&semaphoreForStdout);

    //printf("%s", resultsForStdout.resultsForWrite);
    log_info(getLogger(), "%s", resultsForStdout.resultsForWrite);
}