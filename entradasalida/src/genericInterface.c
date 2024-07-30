#include "genericInterface.h"

void executeIOGenSleepAndSendResults()
{   
    executeIOGenSleep();

    sendIOGenSleepResultsToKernel();

    interfaceData.currentOperation.operation = IO_NULL;
    //free(interfaceData.currentOperation.params);
    interfaceData.currentOperation.pid = -1;
}

void executeIOGenSleep()
{   
    log_info(getLogger(), "PID: %d - Operacion: IO_GEN_SLEEP", (int)interfaceData.currentOperation.pid);
    t_paramsForGenericInterface *params = (t_paramsForGenericInterface*)interfaceData.currentOperation.params;
    usleep(1000 * interfaceData.workUnits * params->workUnits);
}