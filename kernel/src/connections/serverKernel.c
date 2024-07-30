#include "serverKernel.h"

int numberOfIOClients = 0;

sem_t semaphoreForIO;

bool _finishAllServersSignal = false;

// Funcion auxiliar que se ejecuta en cada iteracion de una lista para loggear su valor. Usada para el primer checkpoint.
void listIterator(char *element)
{
    log_info(getLogger(), "%s", element);
}

void receiveClientIteration(int socketServer)
{
    if (_finishAllServersSignal)
    {
        return;
    }

    // Esperar la conexión de un cliente
    int socketClient = waitClient(getLogger(), socketServer);
    if (socketClient == -1)
    {
        log_error(getLogger(), "Error al esperar cliente.\n");
        exit(EXIT_FAILURE);
    }

    // Recibir el codigo de operacion para saber de que tipo es el cliente que se quiere conectar
    operationCode opCode = getOperation(socketClient);
    if (_finishAllServersSignal)
    {
        return;
    }

    switch (opCode)
    {
    case IO_MODULE:

        if (numberOfIOClients >= MAX_IO_CLIENTS)
        {
            log_info(getLogger(), "Un cliente IO se intento conectar. Fue rechazado debido a que se alcanzo la cantidad maxima de clientes.");
            break;
        }

        log_info(getLogger(), "Se conecto un modulo IO");

        initServerForASocket(socketClient, serverKernelForIO);

        sem_wait(&semaphoreForIO);
        numberOfIOClients++;
        sem_post(&semaphoreForIO);

        break;

    case DO_NOTHING:
        break;

    case ERROR:
        log_error(getLogger(), ERROR_CASE_MESSAGE);
        break;

    default:
        log_error(getLogger(), DEFAULT_CASE_MESSAGE);
        break;
    }
}


void serverKernelForMemory(int *socketClient)
{

    bool exitLoop = false;
    while (!exitLoop || _finishAllServersSignal)
    {
        // Recibir el codigo de operacion y hacer la operacion recibida.
        operationCode opCode = getOperation(*socketClient);
        if (_finishAllServersSignal)
        {
            break;
        }

        switch (opCode)
        {
        case PACKAGE_FROM_IO:
            log_info(getLogger(), "Obteniendo paquete por parte del modulo IO");
            t_list *listPackage = getPackage(*socketClient);
            log_info(getLogger(), "Paquete obtenido con exito del modulo IO");
            operationPackageFromIO(listPackage);
            break;

        case MEMORY_SEND_RESPONSE_FOR_NEW_PROCESS:
            memorySendResponseForNewProcess(socketClient);
            break;

        case DO_NOTHING:
            break;

        case ERROR:
            log_error(getLogger(), ERROR_CASE_MESSAGE);
            exitLoop = true;
            break;

        default:
            log_error(getLogger(), DEFAULT_CASE_MESSAGE);
            break;
        }
    }

    free(socketClient);

    sem_wait(&semaphoreForIO);
    numberOfIOClients--;
    sem_post(&semaphoreForIO);
}


void serverKernelForIO(int *socketClient)
{

    bool exitLoop = false;
    while (!exitLoop || _finishAllServersSignal)
    {
        // Recibir el codigo de operacion y hacer la operacion recibida.
        operationCode opCode = getOperation(*socketClient);
        if (_finishAllServersSignal)
        {
            break;
        }

        switch (opCode)
        {
        case PACKAGE_FROM_IO:
            log_info(getLogger(), "Obteniendo paquete por parte del modulo IO");
            t_list *listPackage = getPackage(*socketClient);
            log_info(getLogger(), "Paquete obtenido con exito del modulo IO");
            operationPackageFromIO(listPackage);
            break;

        case DO_NOTHING:
            break;

        case IO_SEND_INTERFACE: 
            ioSendInterface(socketClient);
            break;

        case IO_OK: 
            ioSendEndOperation(socketClient);
            break;

        case ERROR:
            log_error(getLogger(), ERROR_CASE_MESSAGE);
            ioInterfaceDisconnect(socketClient);
            exitLoop = true;
            break;

        default:
            log_error(getLogger(), DEFAULT_CASE_MESSAGE);
            break;
        }
    }

    free(socketClient);

    sem_wait(&semaphoreForIO);
    numberOfIOClients--;
    sem_post(&semaphoreForIO);
}

void serverKernelForCPU(int *socketClient)
{
    bool exitLoop = false;
    while (!exitLoop || _finishAllServersSignal)
    {
        // Recibir el codigo de operacion y hacer la operacion recibida.
        operationCode opCode = getOperation(*socketClient);
        if (_finishAllServersSignal)
        {
            break;
        }

        switch (opCode)
        {

        case CPU_SEND_CONTEXT_FOR_EXIT: 
            cpuSendExitProcess(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_END_PROCESS:
            cpuSendInterruptKillProcess(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_OUT_OF_MEMORY:
            cpuSendOutOfMemoryProcess(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_END_QUANTUM: 
            cpuSendInterruptQ(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_WAIT: 
            cpuSendWaitOfProcess(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_SIGNAL: 
            cpuSendSignalofProcess(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_IO_GENERIC: 
            cpuSendRequestForIOGenSleep(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_IO_STDIN:
            cpuSendRequestForIOStdinRead(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_IO_STDOUT:
            cpuSendRequestForIOStdoutWrite(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_IO_FS_CREATE:
            cpuSendRequestForIODialFsCreate(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_IO_FS_DELETE:
            cpuSendRequestForIODialFsDelete(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_IO_FS_TRUNCATE:
            cpuSendRequestForIODialFsTruncate(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_IO_FS_READ:
            cpuSendRequestForIODialFsRead(socketClient);
            break;
        
        case CPU_SEND_CONTEXT_FOR_IO_FS_WRITE:
            cpuSendRequestForIODialFsWrite(socketClient);
            break;

        case DO_NOTHING:
            break;

        case ERROR:
            log_error(getLogger(), ERROR_CASE_MESSAGE);
            exitLoop = true;
            break;

        default:
            log_error(getLogger(), DEFAULT_CASE_MESSAGE);
            break;
        }
    }

    //free(socketClient);

    sem_wait(&semaphoreForIO);
    numberOfIOClients--;
    sem_post(&semaphoreForIO);
}

void memorySendResponseForNewProcess(int *socketClientMemory)
{
    t_list *listPackage = getPackage(*socketClientMemory);

    bool* pFlagMemoryResponseAux = (bool*)list_remove(listPackage, 0); 

    flagMemoryResponse = *pFlagMemoryResponseAux;

    sem_post(&semMemoryOk);

    free(pFlagMemoryResponseAux);
    list_destroy(listPackage);
}

void operationPackageFromIO(t_list *package)
{
    list_iterate(package, (void*)listIterator);
}

void ioSendInterface(int *socketClientIO)
{
    t_list *listPackage = getPackage(*socketClientIO);

    // Recibo el nombre y tipo de la interfaz.
    char* nameInterface = (char*)list_remove(listPackage, 0);
    interfaceType *typeInterface = (interfaceType*)list_remove(listPackage, 0);
    
    // Creo la interfaz.
    interface_t *newInterface = createInterface(nameInterface, *typeInterface);

    // Guardo el socket, para tener la conexion.
    newInterface->socket = socketClientIO;

    list_push(interfacesList, newInterface);

    free(nameInterface);
    free(typeInterface);
    list_destroy(listPackage);

}

void ioSendEndOperation(int *socketClientIO)
{

    t_list *listPackage = getPackage(*socketClientIO);

    // Recibo el nombre y tipo de la interfaz.
    char* nameInterface = (char*)list_remove(listPackage, 0);

    // Busco la interfaz por el nombre identificador.
    interface_t *interfaceFound = foundInterface(nameInterface);

    pcb_t *processBlockToReady = interfaceFound->processAssign;

    sem_wait(&semPausePlanning);
    sem_post(&semPausePlanning);

    if(!interfaceFound->flagKillProcess){ // Este flag es por si se tiro un finalizar_proceso mientras el proceso estaba en una interfaz realizando una operacion. Caso MUY particular.

        interfaceFound->isBusy = false;
        interfaceFound->processAssign = NULL;

        processBlockToReady->isInInterface = false;

        list_remove_element_mutex(pcbBlockList, processBlockToReady); // Remuevo el proceso de la pcbBlockList.

        if(algorithm != VRR){ //ESTO SI EL ALGORITMO DE PLANIFICACION ES RR O FIFO.

            pthread_mutex_lock(&mutexOrderReadyExecProcess);

            list_push(pcbReadyList, processBlockToReady); // Lo paso a la lista de ready porque ya termino su operacion.

            processBlockToReady->state = PCB_READY;

            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_BLOCK - Estado Actual: PCB_READY", processBlockToReady->pid);

            sem_post(&semReady);

            pthread_mutex_unlock(&mutexOrderReadyExecProcess);

        } else {

            pthread_mutex_lock(&mutexOrderReadyExecProcess);

            list_push(pcbReadyPriorityList, processBlockToReady); // Pasa a pcbReadyPriorityList porque es casi seguro que no se va a dar la casualidad de que haya entrado a block justo en el tiempo que se termino el Quantum.

            processBlockToReady->state = PCB_READY_PLUS;

            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_BLOCK - Estado Actual: PCB_READY_PLUS", processBlockToReady->pid);

            char* listPids = _listPids(pcbReadyPriorityList->list);

            log_info(getLogger(), listPids, "ReadyPlus" ,getKernelConfig()->ALGORITMO_PLANIFICACION);

            free(listPids);

            sem_post(&semExec);

            pthread_mutex_unlock(&mutexOrderReadyExecProcess);

        }

    } else {

        pthread_mutex_lock(&mutexOrderPcbReadyPlus);

        sem_post(&semKillProcessInInterface);

        interfaceFound->isBusy = false;

        interfaceFound->flagKillProcess = false;

        pthread_mutex_unlock(&mutexOrderPcbReadyPlus);

    }


    if(!list_mutex_is_empty(interfaceFound->blockList)){ // Se fija si la interfaz tiene algun otro proceso en espera. 
        pcb_t* processBlocked = list_pop(interfaceFound->blockList);

        interfaceFound->processAssign = processBlocked;
        interfaceFound->isBusy = true;
        
        processBlocked->isInInterface = true;

        t_list *listOfPhysicalAdressesInfo;
        uint32_t amountOfPhysicalAddresses;
        uint32_t sizeToReadOrWrite;
        uint32_t pointer;

        switch (interfaceFound->interfaceType)
        {
        case Generic:
            uint32_t timeOfOperation = processBlocked->params->param1; // Agarro el parametro que tenia. 
            sendIOGenSleepOperationToIO(interfaceFound, timeOfOperation); 
            break;

        case STDIN:
            listOfPhysicalAdressesInfo = processBlocked->params->listAux;
            amountOfPhysicalAddresses = processBlocked->params->param1;
            sizeToReadOrWrite = processBlocked->params->param2;
            sendIOStdinReadOperationToIO(interfaceFound, listOfPhysicalAdressesInfo, amountOfPhysicalAddresses, sizeToReadOrWrite);
            
            for (int i = 0; i < amountOfPhysicalAddresses; i++) {
                physicalAddressInfoP *adresses = list_remove(processBlocked->params->listAux, 0);

                free(adresses->physicalAddress);
                free(adresses->size);
                free(adresses);
            }
            break;
        
        case STDOUT:
            listOfPhysicalAdressesInfo = processBlocked->params->listAux;
            amountOfPhysicalAddresses = processBlocked->params->param1;
            sizeToReadOrWrite = processBlocked->params->param2;
            sendIOStdoutWriteOperationToIO(interfaceFound, listOfPhysicalAdressesInfo, amountOfPhysicalAddresses, sizeToReadOrWrite);
            
            for (int i = 0; i < amountOfPhysicalAddresses; i++) {
                physicalAddressInfoP *adresses = list_remove(processBlocked->params->listAux, 0);

                free(adresses->physicalAddress);
                free(adresses->size);
                free(adresses);
            }
            break;

        case DialFS:

            processBlocked->params->isWaitingFs = false;
            switch (processBlocked->params->typeOpFs)
            {
            case FS_CREATE:
                sendIODialFsCreateOperationToIO(interfaceFound, processBlocked->params->param3);
                free(processBlocked->params->param3);
                break;

            case FS_DELETE:
                sendIODialFsCreateOperationToIO(interfaceFound, processBlocked->params->param3);
                free(processBlocked->params->param3);
                break;

            case FS_TRUNCATE:
                sendIODialFsTruncateOperationToIO(interfaceFound, processBlocked->params->param3, processBlocked->params->param1);
                free(processBlocked->params->param3);
                break;

            case FS_READ:
                listOfPhysicalAdressesInfo = processBlocked->params->listAux;
                amountOfPhysicalAddresses = processBlocked->params->param1;
                sizeToReadOrWrite = processBlocked->params->param2;
                pointer = processBlocked->params->param4;
                sendIODialFsReadOperationToIO(interfaceFound, processBlocked->params->param3, listOfPhysicalAdressesInfo, amountOfPhysicalAddresses, sizeToReadOrWrite, pointer);

                free(processBlocked->params->param3);
                for (int i = 0; i < amountOfPhysicalAddresses; i++) {
                    physicalAddressInfoP *adresses = list_remove(processBlocked->params->listAux, 0);

                    free(adresses->physicalAddress);
                    free(adresses->size);
                    free(adresses);
                }
                break;

            case FS_WRITE:
                listOfPhysicalAdressesInfo = processBlocked->params->listAux;
                amountOfPhysicalAddresses = processBlocked->params->param1;
                sizeToReadOrWrite = processBlocked->params->param2;
                pointer = processBlocked->params->param4;
                sendIODialFsWriteOperationToIO(interfaceFound, processBlocked->params->param3, listOfPhysicalAdressesInfo, amountOfPhysicalAddresses, sizeToReadOrWrite, pointer);

                free(processBlocked->params->param3);
                for (int i = 0; i < amountOfPhysicalAddresses; i++) {
                    physicalAddressInfoP *adresses = list_remove(processBlocked->params->listAux, 0);

                    free(adresses->physicalAddress);
                    free(adresses->size);
                    free(adresses);
                }
                break;

            default:
                break;
            }
        break;

        default:
            break;
        }
    }

    free(nameInterface);
    list_destroy(listPackage);
}

void ioInterfaceDisconnect(int *socketClientIO)
{
    interface_t *interfaceFound = foundInterfaceBySocket(*socketClientIO); //Nunca deberia encontrar nada.

    if (interfaceFound == NULL) log_error(getLogger(), "No deberia ocurrir nunca. Se desconecta una interfaz que nunca se conecto antes.");

    list_remove_element_mutex(interfacesList, interfaceFound);

    free(interfaceFound->name);
    destroyListMutex(interfaceFound->blockList);
    free(interfaceFound);
}

void cpuSendRequestForIOGenSleep(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso 
    pcb_t *processExec = assignContextToPcb(contextProcess);

    // Recibo el nombre de la interfaz a usar.
    char* nameRequestInterface = (char*)list_remove(listPackage, 0);

    // Recibo la cantidad de tiempo a utilizar.
    uint32_t *timeOfOperation = (uint32_t*)list_remove(listPackage, 0);

    if(processExec->processKilled)
    {
        list_push(pcbExecList, processExec);

        sem_post(&semKillProcessExec);

        free(timeOfOperation);
        free(nameRequestInterface);
        list_destroy(listPackage);

        return;
    }

    // Busco la interfaz por el nombre identificador.
    interface_t *interfaceFound = foundInterface(nameRequestInterface);


    if(interfaceFound == NULL){ // Si no existe se manda el proceso a exit.
        list_push(pcbExitList, processExec);
        processExec->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {
        if(interfaceFound->interfaceType != Generic){ // El tipo de interfaz Generic es el unico que puede realizar la operacion IO_GEN_SLEEP. COn verificar que no sea de este tipo directamente no admite la operacion y pasa a exit.
            list_push(pcbExitList, processExec);
            processExec->state = PCB_EXIT;
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

            sem_wait(&semPausePlanning);
            sem_post(&semPausePlanning);

            log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

            sem_post(&semMultiProcessing);
            sem_post(&semExit);

        } else {
            list_push(pcbBlockList, processExec); // Una vez dada las validaciones el kernel envia el proceso a pcbBlockList.
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_BLOCK", processExec->pid);
            log_info(getLogger(), "PID: %d - Bloqueado por: %s", processExec->pid, interfaceFound->name); // Para testeos puse el nombre de la interfaz para que sea mas claro.

            processExec->state = PCB_BLOCK;

            // Detengo el tiempo en el que estuvo en exec si el algoritmo de plani es VRR.
            if(algorithm == VRR) temporal_stop(processExec->quantumForVRR); 

            sem_post(&semMultiProcessing); // Una vez que pasan a pcbBlockList dejan lugar en exec a otro proceso.


            if(!interfaceFound->isBusy){ // Se fija si la interfaz no esta ocupada y lo asigna. 
                interfaceFound->isBusy = true;
                interfaceFound->processAssign = processExec;

                processExec->isInInterface = true;

                sendIOGenSleepOperationToIO(interfaceFound, *timeOfOperation);
                
            } else {
                list_push(interfaceFound->blockList, processExec); // Se agrega el proceso a la lista de espera de esa interfaz.

                processExec->params->param1 = *timeOfOperation; // Se guarda el tiempo de operacion para usarse despues que la interfaz este liberada. 
            }
        }
    }

    free(timeOfOperation);
    free(nameRequestInterface);
    list_destroy(listPackage);

}

void cpuSendRequestForIOStdinRead(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso 
    pcb_t *processExec = assignContextToPcb(contextProcess);

    // Recibo el nombre de la interfaz a usar.
    char* nameRequestInterface = list_remove(listPackage, 0);

    int *amountOfPhysicalAddresses = (int*)list_remove(listPackage, 0);

    t_list *listOfPhysicalAddresses = list_create();

    for (int i = 0; i < *amountOfPhysicalAddresses; i++)
    {
        physicalAddressInfoP *adresses = malloc(sizeof(physicalAddressInfoP));

        adresses->physicalAddress = (int*)list_remove(listPackage, 0);
        adresses->size = (int*)list_remove(listPackage, 0);

        list_add(listOfPhysicalAddresses, adresses);
    }

    int *sizeToReadOrWrite = list_remove(listPackage, 0);

    if(processExec->processKilled)
    {
        list_push(pcbExecList, processExec);

        sem_post(&semKillProcessExec);

        free(nameRequestInterface);
    
        for(int i = 0; i < *amountOfPhysicalAddresses; i++)
        {
            physicalAddressInfoP *adresses = list_remove(listOfPhysicalAddresses, 0);

            free(adresses->physicalAddress);
            free(adresses->size);
            free(adresses);
        }

        free(amountOfPhysicalAddresses);
        list_destroy(listOfPhysicalAddresses);
        free(sizeToReadOrWrite);

        list_destroy(listPackage);

        return;

    }

    // Busco la interfaz por el nombre identificador.
    interface_t *interfaceFound = foundInterface(nameRequestInterface);

    if(interfaceFound == NULL){ // Si no existe se manda el proceso a exit.
        list_push(pcbExitList, processExec);
        processExec->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);
        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {
        if(interfaceFound->interfaceType != STDIN){ // El tipo de interfaz STDIN es el unico que puede realizar la operacion IO_STDIN_READ. COn verificar que no sea de este tipo directamente no admite la operacion y pasa a exit.
            list_push(pcbExitList, processExec);
            processExec->state = PCB_EXIT;
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);
            log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

            sem_post(&semMultiProcessing);
            sem_post(&semExit);

        } else {
            list_push(pcbBlockList, processExec); // Una vez dada las validaciones el kernel envia el proceso a pcbBlockList.
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_BLOCK", processExec->pid);
            log_info(getLogger(), "PID: %d - Bloqueado por: %s", processExec->pid, interfaceFound->name); // Para testeos puse el nombre de la interfaz para que sea mas claro.

            processExec->state = PCB_BLOCK;

            // Detengo el tiempo en el que estuvo en exec si el algoritmo de plani es VRR.
            if(algorithm == VRR) temporal_stop(processExec->quantumForVRR); 

            sem_post(&semMultiProcessing);; // Una vez que pasan a pcbBlockList dejan lugar en exec a otro proceso.
            
            if(!interfaceFound->isBusy){ // Se fija si la interfaz no esta ocupada y lo asigna. 
                interfaceFound->isBusy = true;
                interfaceFound->processAssign = processExec;

                processExec->isInInterface = true;

                sendIOStdinReadOperationToIO(interfaceFound, listOfPhysicalAddresses, *amountOfPhysicalAddresses, *sizeToReadOrWrite);
                
            } else {
                list_push(interfaceFound->blockList, processExec); // Se agrega el proceso a la lista de espera de esa interfaz.

                processExec->params->param1 = *amountOfPhysicalAddresses;
                processExec->params->param2 = *sizeToReadOrWrite;

                // Se guarda la informacion de la lista para tenerla al momento que se desoscupe la interfaz.
                for (int i = 0; i < *amountOfPhysicalAddresses; i++)
                {
                    physicalAddressInfoP *adresses = malloc(sizeof(physicalAddressInfoP));
                    adresses->physicalAddress = malloc(sizeof(int));
                    adresses->size = malloc(sizeof(int));

                    physicalAddressInfoP *adressesAux = list_get(listOfPhysicalAddresses, i);

                    *(adresses->physicalAddress) = *(adressesAux->physicalAddress);
                    *(adresses->size) = *(adressesAux->size);

                    list_add(processExec->params->listAux, adresses);
                }

            }
        }
    }

    free(nameRequestInterface);
    
    for(int i = 0; i < *amountOfPhysicalAddresses; i++){
        physicalAddressInfoP *adresses = list_remove(listOfPhysicalAddresses, 0);

        free(adresses->physicalAddress);
        free(adresses->size);
        free(adresses);
    }

    free(amountOfPhysicalAddresses);
    list_destroy(listOfPhysicalAddresses);
    free(sizeToReadOrWrite);

    list_destroy(listPackage);


}

void cpuSendRequestForIOStdoutWrite(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso 
    pcb_t *processExec = assignContextToPcb(contextProcess);

    // Recibo el nombre de la interfaz a usar.
    char* nameRequestInterface = list_remove(listPackage, 0);

    int *amountOfPhysicalAddresses = (int*)list_remove(listPackage, 0);

    t_list *listOfPhysicalAddresses = list_create();

    for (int i = 0; i < *amountOfPhysicalAddresses; i++)
    {
        physicalAddressInfoP *adresses = malloc(sizeof(physicalAddressInfoP));

        adresses->physicalAddress = (int*)list_remove(listPackage, 0);
        adresses->size = (int*)list_remove(listPackage, 0);

        list_add(listOfPhysicalAddresses, adresses);
    }

    int *sizeToReadOrWrite = list_remove(listPackage, 0);


    if(processExec->processKilled)
    {
        list_push(pcbExecList, processExec);

        sem_post(&semKillProcessExec);

        free(nameRequestInterface);
    
        for(int i = 0; i < *amountOfPhysicalAddresses; i++)
        {
            physicalAddressInfoP *adresses = list_remove(listOfPhysicalAddresses, 0);

            free(adresses->physicalAddress);
            free(adresses->size);
            free(adresses);
        }

        free(amountOfPhysicalAddresses);
        list_destroy(listOfPhysicalAddresses);
        free(sizeToReadOrWrite);

        list_destroy(listPackage);

        return;

    }

    // Busco la interfaz por el nombre identificador.
    interface_t *interfaceFound = foundInterface(nameRequestInterface);

    if(interfaceFound == NULL){ // Si no existe se manda el proceso a exit.
        list_push(pcbExitList, processExec);
        processExec->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);
        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {
        if(interfaceFound->interfaceType != STDOUT){ // El tipo de interfaz STDIN es el unico que puede realizar la operacion IO_STDIN_READ. COn verificar que no sea de este tipo directamente no admite la operacion y pasa a exit.
            list_push(pcbExitList, processExec);
            processExec->state = PCB_EXIT;
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);
            log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

            sem_post(&semMultiProcessing);
            sem_post(&semExit);

        } else {
            list_push(pcbBlockList, processExec); // Una vez dada las validaciones el kernel envia el proceso a pcbBlockList.
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_BLOCK", processExec->pid);
            log_info(getLogger(), "PID: %d - Bloqueado por: %s", processExec->pid, interfaceFound->name); // Para testeos puse el nombre de la interfaz para que sea mas claro.

            processExec->state = PCB_BLOCK;

            // Detengo el tiempo en el que estuvo en exec si el algoritmo de plani es VRR.
            if(algorithm == VRR) temporal_stop(processExec->quantumForVRR); 

            sem_post(&semMultiProcessing);; // Una vez que pasan a pcbBlockList dejan lugar en exec a otro proceso.
            
            if(!interfaceFound->isBusy){ // Se fija si la interfaz no esta ocupada y lo asigna. 
                interfaceFound->isBusy = true;
                interfaceFound->processAssign = processExec;

                processExec->isInInterface = true;

                sendIOStdoutWriteOperationToIO(interfaceFound, listOfPhysicalAddresses, *amountOfPhysicalAddresses, *sizeToReadOrWrite);
                
            } else {
                list_push(interfaceFound->blockList, processExec); // Se agrega el proceso a la lista de espera de esa interfaz.

                processExec->params->param1 = *amountOfPhysicalAddresses;
                processExec->params->param2 = *sizeToReadOrWrite;

                // Se guarda la informacion de la lista para tenerla al momento que se desoscupe la interfaz.
                for (int i = 0; i < *amountOfPhysicalAddresses; i++)
                {
                    physicalAddressInfoP *adresses = malloc(sizeof(physicalAddressInfoP));
                    adresses->physicalAddress = malloc(sizeof(int));
                    adresses->size = malloc(sizeof(int));

                    physicalAddressInfoP *adressesAux = list_get(listOfPhysicalAddresses, i);

                    *(adresses->physicalAddress) = *(adressesAux->physicalAddress);
                    *(adresses->size) = *(adressesAux->size);

                    list_add(processExec->params->listAux, adresses);
                }
            }
        }
    }

    free(nameRequestInterface);
    
    for(int i = 0; i < *amountOfPhysicalAddresses; i++){
        physicalAddressInfoP *adresses = list_remove(listOfPhysicalAddresses, 0);

        free(adresses->physicalAddress);
        free(adresses->size);
        free(adresses);
    }

    free(amountOfPhysicalAddresses);
    list_destroy(listOfPhysicalAddresses);
    free(sizeToReadOrWrite);

    list_destroy(listPackage);

}

void cpuSendRequestForIODialFsCreate(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso popeado de pcbExecList
    pcb_t *processExec = assignContextToPcb(contextProcess);

    // Recibo el nombre de la interfaz a usar.
    char* nameRequestInterface = list_remove(listPackage, 0);

    interface_t* interfaceFound = foundInterface(nameRequestInterface);

    char* nameFileForCreate = list_remove(listPackage, 0);

    if(interfaceFound == NULL){ // Si no existe se manda el proceso a exit.
        list_push(pcbExitList, processExec);
        processExec->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {
        if(interfaceFound->interfaceType != DialFS){ // El tipo de interfaz DialFs es el unico que puede realizar la operacion Create. COn verificar que no sea de este tipo directamente no admite la operacion y pasa a exit.
            list_push(pcbExitList, processExec);
            processExec->state = PCB_EXIT;
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

            sem_wait(&semPausePlanning);
            sem_post(&semPausePlanning);

            log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

            sem_post(&semMultiProcessing);
            sem_post(&semExit);

        } else {
            list_push(pcbBlockList, processExec); // Una vez dada las validaciones el kernel envia el proceso a pcbBlockList.
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_BLOCK", processExec->pid);
            log_info(getLogger(), "PID: %d - Bloqueado por: %s", processExec->pid, interfaceFound->name); // Para testeos puse el nombre de la interfaz para que sea mas claro.

            processExec->state = PCB_BLOCK;

            // Detengo el tiempo en el que estuvo en exec si el algoritmo de plani es VRR.
            if(algorithm == VRR) temporal_stop(processExec->quantumForVRR); 

            sem_post(&semMultiProcessing); // Una vez que pasan a pcbBlockList dejan lugar en exec a otro proceso.


            if(!interfaceFound->isBusy){ // Se fija si la interfaz no esta ocupada y lo asigna. 
                interfaceFound->isBusy = true;
                interfaceFound->processAssign = processExec;

                processExec->params->isWaitingFs = false;
                processExec->isInInterface = true;

                sendIODialFsCreateOperationToIO(interfaceFound, nameFileForCreate);
                
            } else {

                list_push(interfaceFound->blockList, processExec); // Se agrega el proceso a la lista de espera de esa interfaz.

                processExec->params->isWaitingFs = true;
                processExec->params->typeOpFs = FS_CREATE;
                processExec->params->param3 = malloc(string_length(nameFileForCreate) + 1);
                strcpy(processExec->params->param3, nameFileForCreate);

            }
        }
    }

    free(nameRequestInterface);
    free(nameFileForCreate);

    list_destroy(listPackage);
}

void cpuSendRequestForIODialFsDelete(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso popeado de pcbExecList
    pcb_t *processExec = assignContextToPcb(contextProcess);

    // Recibo el nombre de la interfaz a usar.
    char* nameRequestInterface = list_remove(listPackage, 0);

    interface_t* interfaceFound = foundInterface(nameRequestInterface);

    char* nameFileForDelete = list_remove(listPackage, 0);
    
    if(interfaceFound == NULL){ // Si no existe se manda el proceso a exit.
        list_push(pcbExitList, processExec);
        processExec->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {
        if(interfaceFound->interfaceType != DialFS){ // El tipo de interfaz DialFs es el unico que puede realizar la operacion Create. COn verificar que no sea de este tipo directamente no admite la operacion y pasa a exit.
            list_push(pcbExitList, processExec);
            processExec->state = PCB_EXIT;
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

            sem_wait(&semPausePlanning);
            sem_post(&semPausePlanning);

            log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

            sem_post(&semMultiProcessing);
            sem_post(&semExit);

        } else {
            list_push(pcbBlockList, processExec); // Una vez dada las validaciones el kernel envia el proceso a pcbBlockList.
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_BLOCK", processExec->pid);
            log_info(getLogger(), "PID: %d - Bloqueado por: %s", processExec->pid, interfaceFound->name); // Para testeos puse el nombre de la interfaz para que sea mas claro.

            processExec->state = PCB_BLOCK;

            // Detengo el tiempo en el que estuvo en exec si el algoritmo de plani es VRR.
            if(algorithm == VRR) temporal_stop(processExec->quantumForVRR); 

            sem_post(&semMultiProcessing); // Una vez que pasan a pcbBlockList dejan lugar en exec a otro proceso.


            if(!interfaceFound->isBusy){ // Se fija si la interfaz no esta ocupada y lo asigna. 
                interfaceFound->isBusy = true;
                interfaceFound->processAssign = processExec;

                processExec->params->isWaitingFs = false;
                processExec->isInInterface = true;

                sendIODialFsDeleteOperationToIO(interfaceFound, nameFileForDelete);
                
            } else {

                list_push(interfaceFound->blockList, processExec); // Se agrega el proceso a la lista de espera de esa interfaz.

                processExec->params->isWaitingFs = true;
                processExec->params->typeOpFs = FS_DELETE;
                processExec->params->param3 = malloc(string_length(nameFileForDelete) + 1);
                strcpy(processExec->params->param3, nameFileForDelete);

            }
        }
    }


    free(nameRequestInterface);
    free(nameFileForDelete);

    list_destroy(listPackage);

}

void cpuSendRequestForIODialFsTruncate(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso popeado de pcbExecList
    pcb_t *processExec = assignContextToPcb(contextProcess);

    // Recibo el nombre de la interfaz a usar.
    char* nameRequestInterface = list_remove(listPackage, 0);

    interface_t* interfaceFound = foundInterface(nameRequestInterface);

    char* nameOfFile = list_remove(listPackage, 0);

    uint32_t *size = list_remove(listPackage, 0);

    if(interfaceFound == NULL){ // Si no existe se manda el proceso a exit.
        list_push(pcbExitList, processExec);
        processExec->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {
        if(interfaceFound->interfaceType != DialFS){ // El tipo de interfaz DialFs es el unico que puede realizar la operacion Create. COn verificar que no sea de este tipo directamente no admite la operacion y pasa a exit.
            list_push(pcbExitList, processExec);
            processExec->state = PCB_EXIT;
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

            sem_wait(&semPausePlanning);
            sem_post(&semPausePlanning);

            log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

            sem_post(&semMultiProcessing);
            sem_post(&semExit);

        } else {
            list_push(pcbBlockList, processExec); // Una vez dada las validaciones el kernel envia el proceso a pcbBlockList.
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_BLOCK", processExec->pid);
            log_info(getLogger(), "PID: %d - Bloqueado por: %s", processExec->pid, interfaceFound->name); // Para testeos puse el nombre de la interfaz para que sea mas claro.

            processExec->state = PCB_BLOCK;

            // Detengo el tiempo en el que estuvo en exec si el algoritmo de plani es VRR.
            if(algorithm == VRR) temporal_stop(processExec->quantumForVRR); 

            sem_post(&semMultiProcessing); // Una vez que pasan a pcbBlockList dejan lugar en exec a otro proceso.


            if(!interfaceFound->isBusy){ // Se fija si la interfaz no esta ocupada y lo asigna. 
                interfaceFound->isBusy = true;
                interfaceFound->processAssign = processExec;

                processExec->params->isWaitingFs = false;
                processExec->isInInterface = true;

                sendIODialFsTruncateOperationToIO(interfaceFound, nameOfFile, *size);
                
            } else {
                list_push(interfaceFound->blockList, processExec); // Se agrega el proceso a la lista de espera de esa interfaz.

                processExec->params->isWaitingFs = true;
                processExec->params->typeOpFs = FS_TRUNCATE;
                processExec->params->param3 = malloc(string_length(nameOfFile) + 1);
                strcpy(processExec->params->param3, nameOfFile);
                processExec->params->param1 = *size;
            }
        }
    }

    free(nameRequestInterface);
    free(nameOfFile);
    free(size);

    list_destroy(listPackage);
}

void cpuSendRequestForIODialFsRead(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso popeado de pcbExecList
    pcb_t *processExec = assignContextToPcb(contextProcess);

    // Recibo el nombre de la interfaz a usar.
    char* nameRequestInterface = list_remove(listPackage, 0);

    interface_t* interfaceFound = foundInterface(nameRequestInterface);

    char* nameOfFile = list_remove(listPackage, 0);

    int *amountOfPhysicalAddresses = (int*)list_remove(listPackage, 0);

    t_list *listOfPhysicalAddresses = list_create();

    for (int i = 0; i < *amountOfPhysicalAddresses; i++)
    {
        physicalAddressInfoP *adresses = malloc(sizeof(physicalAddressInfoP));

        adresses->physicalAddress = (int*)list_remove(listPackage, 0);
        adresses->size = (int*)list_remove(listPackage, 0);

        list_add(listOfPhysicalAddresses, adresses);
    }

    int *sizeToReadOrWrite = list_remove(listPackage, 0);

    int *pointer = list_remove(listPackage, 0);

    if(interfaceFound == NULL){ // Si no existe se manda el proceso a exit.
        list_push(pcbExitList, processExec);
        processExec->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {
        if(interfaceFound->interfaceType != DialFS){ // El tipo de interfaz DialFs es el unico que puede realizar la operacion Create. COn verificar que no sea de este tipo directamente no admite la operacion y pasa a exit.
            list_push(pcbExitList, processExec);
            processExec->state = PCB_EXIT;
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

            sem_wait(&semPausePlanning);
            sem_post(&semPausePlanning);

            log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

            sem_post(&semMultiProcessing);
            sem_post(&semExit);

        } else {
            list_push(pcbBlockList, processExec); // Una vez dada las validaciones el kernel envia el proceso a pcbBlockList.
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_BLOCK", processExec->pid);
            log_info(getLogger(), "PID: %d - Bloqueado por: %s", processExec->pid, interfaceFound->name); // Para testeos puse el nombre de la interfaz para que sea mas claro.

            processExec->state = PCB_BLOCK;

            // Detengo el tiempo en el que estuvo en exec si el algoritmo de plani es VRR.
            if(algorithm == VRR) temporal_stop(processExec->quantumForVRR); 

            sem_post(&semMultiProcessing); // Una vez que pasan a pcbBlockList dejan lugar en exec a otro proceso.


            if(!interfaceFound->isBusy){ // Se fija si la interfaz no esta ocupada y lo asigna. 
                interfaceFound->isBusy = true;
                interfaceFound->processAssign = processExec;

                processExec->params->isWaitingFs = false;
                processExec->isInInterface = true;

                sendIODialFsReadOperationToIO(interfaceFound, nameOfFile, listOfPhysicalAddresses, *amountOfPhysicalAddresses, *sizeToReadOrWrite, *pointer);
                
            } else {
                list_push(interfaceFound->blockList, processExec); // Se agrega el proceso a la lista de espera de esa interfaz.

                processExec->params->isWaitingFs = true;
                processExec->params->typeOpFs = FS_READ;
                processExec->params->param3 = malloc(string_length(nameOfFile) + 1);
                strcpy(processExec->params->param3, nameOfFile);
                processExec->params->param1 = *amountOfPhysicalAddresses;
                
                // Se guarda la informacion de la lista para tenerla al momento que se desoscupe la interfaz.
                for (int i = 0; i < *amountOfPhysicalAddresses; i++)
                {
                    physicalAddressInfoP *adresses = malloc(sizeof(physicalAddressInfoP));
                    adresses->physicalAddress = malloc(sizeof(int));
                    adresses->size = malloc(sizeof(int));

                    physicalAddressInfoP *adressesAux = list_get(listOfPhysicalAddresses, i);

                    *(adresses->physicalAddress) = *(adressesAux->physicalAddress);
                    *(adresses->size) = *(adressesAux->size);

                    list_add(processExec->params->listAux, adresses);
                }

                processExec->params->param2 = *sizeToReadOrWrite;
                processExec->params->param4 = *pointer;

            }
        }
    }

    free(nameRequestInterface);
    free(nameOfFile);
    
    for(int i = 0; i < *amountOfPhysicalAddresses; i++){
        physicalAddressInfoP *adresses = list_remove(listOfPhysicalAddresses, 0);

        free(adresses->physicalAddress);
        free(adresses->size);
        free(adresses);
    }

    free(amountOfPhysicalAddresses);
    list_destroy(listOfPhysicalAddresses);
    free(sizeToReadOrWrite);
    free(pointer);

    list_destroy(listPackage);
}


void cpuSendRequestForIODialFsWrite(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso popeado de pcbExecList
    pcb_t *processExec = assignContextToPcb(contextProcess);

    // Recibo el nombre de la interfaz a usar.
    char* nameRequestInterface = list_remove(listPackage, 0);

    interface_t* interfaceFound = foundInterface(nameRequestInterface);

    char* nameOfFile = list_remove(listPackage, 0);

    int *amountOfPhysicalAddresses = (int*)list_remove(listPackage, 0);

    t_list *listOfPhysicalAddresses = list_create();

    for (int i = 0; i < *amountOfPhysicalAddresses; i++)
    {
        physicalAddressInfoP *adresses = malloc(sizeof(physicalAddressInfoP));

        adresses->physicalAddress = (int*)list_remove(listPackage, 0);
        adresses->size = (int*)list_remove(listPackage, 0);

        list_add(listOfPhysicalAddresses, adresses);
    }

    int *sizeToReadOrWrite = list_remove(listPackage, 0);

    int *pointer = list_remove(listPackage, 0);

    if(interfaceFound == NULL){ // Si no existe se manda el proceso a exit.
        list_push(pcbExitList, processExec);
        processExec->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {
        if(interfaceFound->interfaceType != DialFS){ // El tipo de interfaz DialFs es el unico que puede realizar la operacion Create. COn verificar que no sea de este tipo directamente no admite la operacion y pasa a exit.
            list_push(pcbExitList, processExec);
            processExec->state = PCB_EXIT;
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

            sem_wait(&semPausePlanning);
            sem_post(&semPausePlanning);

            log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

            sem_post(&semMultiProcessing);
            sem_post(&semExit);

        } else {
            list_push(pcbBlockList, processExec); // Una vez dada las validaciones el kernel envia el proceso a pcbBlockList.
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_BLOCK", processExec->pid);
            log_info(getLogger(), "PID: %d - Bloqueado por : %s", processExec->pid, interfaceFound->name); // Para testeos puse el nombre de la interfaz para que sea mas claro.

            processExec->state = PCB_BLOCK;

            // Detengo el tiempo en el que estuvo en exec si el algoritmo de plani es VRR.
            if(algorithm == VRR) temporal_stop(processExec->quantumForVRR); 

            sem_post(&semMultiProcessing); // Una vez que pasan a pcbBlockList dejan lugar en exec a otro proceso.


            if(!interfaceFound->isBusy){ // Se fija si la interfaz no esta ocupada y lo asigna. 
                interfaceFound->isBusy = true;
                interfaceFound->processAssign = processExec;

                processExec->isInInterface = true;
                processExec->params->isWaitingFs = false;

                sendIODialFsWriteOperationToIO(interfaceFound, nameOfFile, listOfPhysicalAddresses, *amountOfPhysicalAddresses, *sizeToReadOrWrite, *pointer);
                
            } else {
                list_push(interfaceFound->blockList, processExec); // Se agrega el proceso a la lista de espera de esa interfaz.

                processExec->params->isWaitingFs = true;
                processExec->params->typeOpFs = FS_WRITE;
                processExec->params->param3 = malloc(string_length(nameOfFile) + 1);
                strcpy(processExec->params->param3, nameOfFile);
                processExec->params->param1 = *amountOfPhysicalAddresses;
                
                // Se guarda la informacion de la lista para tenerla al momento que se desoscupe la interfaz.
                for (int i = 0; i < *amountOfPhysicalAddresses; i++)
                {
                    physicalAddressInfoP *adresses = malloc(sizeof(physicalAddressInfoP));
                    adresses->physicalAddress = malloc(sizeof(int));
                    adresses->size = malloc(sizeof(int));

                    physicalAddressInfoP *adressesAux = list_get(listOfPhysicalAddresses, i);

                    *(adresses->physicalAddress) = *(adressesAux->physicalAddress);
                    *(adresses->size) = *(adressesAux->size);

                    list_add(processExec->params->listAux, adresses);
                }

                processExec->params->param2 = *sizeToReadOrWrite;
                processExec->params->param4 = *pointer;

            }
        }
    }

    free(nameRequestInterface);
    free(nameOfFile);
    
    for(int i = 0; i < *amountOfPhysicalAddresses; i++){
        physicalAddressInfoP *adresses = list_remove(listOfPhysicalAddresses, 0);

        free(adresses->physicalAddress);
        free(adresses->size);
        free(adresses);
    }

    free(amountOfPhysicalAddresses);
    list_destroy(listOfPhysicalAddresses);
    free(sizeToReadOrWrite);
    free(pointer);

    list_destroy(listPackage);
}

void cpuSendOutOfMemoryProcess(int *socketClientCPUDispatch)
{

    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso popeado de pcbExecList
    pcb_t *processExecToExit = assignContextToPcb(contextProcess);

    log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExecToExit->pid);

    processExecToExit->state = PCB_EXIT;

    sem_wait(&semPausePlanning);
    sem_post(&semPausePlanning);

    log_info(getLogger(), "Finaliza el proceso %d - Motivo: OUT_OF_MEMORY", processExecToExit->pid);

    list_push(pcbExitList, processExecToExit);

    list_destroy(listPackage);

    sem_post(&semExit);
    sem_post(&semMultiProcessing);

}

void finishAllServersSignal()
{
    _finishAllServersSignal = true;
}

void cpuSendExitProcess(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso popeado de pcbExecList
    pcb_t *processExecToExit = assignContextToPcb(contextProcess);

    log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExecToExit->pid);

    processExecToExit->state = PCB_EXIT;

    sem_wait(&semPausePlanning);
    sem_post(&semPausePlanning);

    log_info(getLogger(), "Finaliza el proceso %d - Motivo: SUCCESS", processExecToExit->pid);

    list_push(pcbExitList, processExecToExit);

    list_destroy(listPackage);

    sem_post(&semExit);
    sem_post(&semMultiProcessing);
}

void cpuSendInterruptKillProcess(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso popeado en Exec.
    pcb_t *processExecToExit = assignContextToPcb(contextProcess);

    list_push(pcbExecList, processExecToExit);

    sem_post(&semKillProcessExec);

    list_destroy(listPackage);

}

void cpuSendInterruptQ(int *socketClientCPUDispatch)
{
    pthread_mutex_lock(&mutexOrderReadyExecProcess);

    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso popeado en Exec.
    pcb_t *processExecToReady = assignContextToPcb(contextProcess);

    list_push(pcbReadyList, processExecToReady);
    processExecToReady->state = PCB_READY;

    log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_READY", processExecToReady->pid);

    sem_wait(&semPausePlanning);
    sem_post(&semPausePlanning);

    log_info(getLogger(), "PID: %d - Desalojado por fin de Quantum", processExecToReady->pid);

    list_destroy(listPackage);

    sem_post(&semMultiProcessing);
    sem_post(&semReady);

    pthread_mutex_unlock(&mutexOrderReadyExecProcess);
}

void cpuSendWaitOfProcess(int *socketClientCPUDispatch)
{
    
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso popeado en Exec
    pcb_t *processExec = assignContextToPcb(contextProcess);

    char* resourceName = (char*)list_remove(listPackage, 0);

    if(processExec->processKilled)
    {
        list_push(pcbExecList, processExec);

        sem_post(&semKillProcessExec);

        free(resourceName);
        list_destroy(listPackage);

        return;
    }

    resource_t* resourceFound = foundResource(resourceName);


    if(resourceFound == NULL){ //Si el recurso pedido no existe, el proceso pasa a exit y se destruye.

        list_push(pcbExitList, processExec);
        processExec->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_RESOURCE", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {

        subtractInstanceResource(resourceFound); // Resta una instancia del recurso.

        if(resourceFound->instances >= 0){ //Si el recurso queda >= 0 se le asigna ese recurso al proceso

            list_push(processExec->resources, resourceFound); // Asigna el recurso al proceso

            list_push(pcbExecList, processExec); // El proceso tiene que volver devuelta a exec PORQUE PUEDE. (Podria generar segmentacion fault sino)

            sendContextToCPU(processExec); // Manda el contexto devuelta a la Cpu para que siga ejecutando.

        } else { 
            list_push(resourceFound->blockList, processExec); // Pasa a la lista de bloqueados del recurso, esperando que se libere.

            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_BLOCK", processExec->pid);
            processExec->state = PCB_BLOCK;
            list_push(pcbBlockList, processExec);
            log_info(getLogger(), "PID: %d - Bloqueado por: %s", processExec->pid, resourceFound->name);

            sem_post(&semMultiProcessing);
            
        }
    }

    free(resourceName);
    list_destroy(listPackage);

}

void cpuSendSignalofProcess(int *socketClientCPUDispatch)
{
    pthread_mutex_lock(&mutexOrderReadyExecProcess);
    
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso 
    pcb_t *processExec = assignContextToPcb(contextProcess);

    char* resourceName = (char*)list_remove(listPackage, 0);

    resource_t* resourceFound = foundResource(resourceName);

    if(resourceFound == NULL){ //Si el recurso pedido no existe, el proceso pasa a exit y se destruye.
        list_push(pcbExitList, processExec);
        processExec->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_RESOURCE", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {
        addInstanceResource(resourceFound); // Suma uno a la instancia del recurso.

        // Se fija si el proceso tenia el recurso asignado, si lo tiene lo libera.
        list_remove_element_mutex(processExec->resources, resourceFound);

        // Se fija si hay algun proceso que este bloqueado para asignarlo por el signal.
        if(list_mutex_size(resourceFound->blockList) > 0){
            pcb_t* processBlockToReady = list_pop(resourceFound->blockList);
            list_push(processBlockToReady->resources, resourceFound);

            list_remove_element_mutex(pcbBlockList, processBlockToReady);

            processBlockToReady->state = PCB_READY;
            list_push(pcbReadyList, processBlockToReady);
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_BLOCK - Estado Actual: PCB_READY", processBlockToReady->pid);

            sem_post(&semReady);

        }
        list_push(pcbExecList, processExec); // El proceso tiene que volver devuelta a exec PORQUE PUEDE. (Podria generar segmentacion fault sino)

        sendContextToCPU(processExec);

        sem_post(&semBlock);  // No hace nada pero me quedaba bonito. :s
    }

    free(resourceName);
    list_destroy(listPackage);

    pthread_mutex_unlock(&mutexOrderReadyExecProcess);
}

contextProcess recieveContextFromPackage(t_list* package)
{
    contextProcess contextProcess;
    
    // Recibe todo el contexto del proceso ejecutando de CPU

    uint32_t *pc = (uint32_t *)list_remove(package, 0);
    uint8_t *ax = (uint8_t *)list_remove(package, 0);
    uint8_t *bx = (uint8_t *)list_remove(package, 0);
    uint8_t *cx = (uint8_t *)list_remove(package, 0);
    uint8_t *dx = (uint8_t *)list_remove(package, 0);
    uint32_t *eax = (uint32_t *)list_remove(package, 0);
    uint32_t *ebx = (uint32_t *)list_remove(package, 0);
    uint32_t *ecx = (uint32_t *)list_remove(package, 0);
    uint32_t *edx = (uint32_t *)list_remove(package, 0);
    uint32_t *di = (uint32_t *)list_remove(package, 0);
    uint32_t *si =  (uint32_t *)list_remove(package, 0);

    contextProcess.pc = *pc;
    contextProcess.registersCpu.AX = *ax;
    contextProcess.registersCpu.BX = *bx;
    contextProcess.registersCpu.CX = *cx;
    contextProcess.registersCpu.DX = *dx;
    contextProcess.registersCpu.EAX = *eax;
    contextProcess.registersCpu.EBX = *ebx;
    contextProcess.registersCpu.ECX = *ecx;
    contextProcess.registersCpu.EDX = *edx;
    contextProcess.registersCpu.DI = *di;
    contextProcess.registersCpu.SI = *si;

    free(pc);
    free(ax);
    free(bx);
    free(cx);
    free(dx);
    free(eax);
    free(ebx);
    free(ecx);
    free(edx);
    free(di);
    free(si);

    return contextProcess;
}

pcb_t* assignContextToPcb(contextProcess contextProcess)
{
    pcb_t* process = list_pop(pcbExecList);

    // Asigna todo el contexto al proceso ejecutando
    process->pc = contextProcess.pc;
    process->registersCpu->AX = contextProcess.registersCpu.AX;
    process->registersCpu->BX = contextProcess.registersCpu.BX;
    process->registersCpu->CX = contextProcess.registersCpu.CX;
    process->registersCpu->DX = contextProcess.registersCpu.DX;
    process->registersCpu->EAX = contextProcess.registersCpu.EAX;
    process->registersCpu->EBX = contextProcess.registersCpu.EBX;
    process->registersCpu->ECX = contextProcess.registersCpu.ECX;
    process->registersCpu->EDX = contextProcess.registersCpu.EDX;
    process->registersCpu->DI = contextProcess.registersCpu.DI;
    process->registersCpu->SI = contextProcess.registersCpu.SI;

    return process;
}
