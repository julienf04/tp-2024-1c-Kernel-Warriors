#include "longTermPlanning.h"
#include "pthread.h"


//Hilo aparte que esta a la espera de que llegue la creacion de
//un proceso y lo mande a la lista de NEW
void newState()
{
    while(1)
    {

        sem_wait(&semMultiProgramming);
        sem_wait(&semNew);


        if(!list_mutex_is_empty(pcbNewList)){

        pthread_mutex_lock(&mutexOrderReadyExecProcess);

        pcb_t* pcbToReady = list_pop(pcbNewList);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);


        list_push(pcbReadyList, pcbToReady);
        pcbToReady->state = PCB_READY;

        //Log obligatorio
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_NEW - Estado Actual: PCB_READY", pcbToReady->pid);
        sem_post(&semReady);

        pthread_mutex_unlock(&mutexOrderReadyExecProcess);
        }

    }
}

//Espera la llegada de procesos a EXIT
void exitState()
{
    while(1){

        sem_wait(&semExit);
        
        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        pcb_t *process = list_pop(pcbExitList);

        pcbState_t prevState = process->state;

        process->state = PCB_EXIT;

        // Pido a memoria que libere todo lo asociado al proceso.
        sendEndProcessToMemory(process);
        destroyProcess(process);

        if(prevState != PCB_NEW){
            if(diffBetweenNewAndPrevMultiprogramming > 0){
                diffBetweenNewAndPrevMultiprogramming --;
            } else {
                sem_post(&semMultiProgramming);
            }
        }

    }
}

//Inicia el planificador a largo plazo
void initLongTermPlanning(){
    pthread_t newStateThread;
    pthread_t exitStateThread;
    pthread_create(&newStateThread, NULL, (void*)newState, NULL);
    pthread_create(&exitStateThread, NULL, (void*)exitState, NULL);
    pthread_detach(newStateThread);
    pthread_detach(exitStateThread);
}

//Crea un proceso
pcb_t* createProcess()
{
    pcb_t *process = malloc(sizeof(pcb_t));
    sem_wait(&semAddPid);
    process->pid = ++pid;
    sem_post(&semAddPid);
    process->processKilled = false;
    process->pc = 0;
    process->state = PCB_NEW;
    process->isInInterface = false;
    process->resources = initListMutex();
    process->quantumForVRR = temporal_create();
    temporal_stop(process->quantumForVRR);
    process->registersCpu = malloc(sizeof(t_registers));
    process->registersCpu->AX = 0;
    process->registersCpu->BX = 0;
    process->registersCpu->CX = 0;
    process->registersCpu->DX = 0;
    process->registersCpu->EAX = 0;
    process->registersCpu->EBX = 0;
    process->registersCpu->ECX = 0;
    process->registersCpu->EDX = 0;
    process->registersCpu->SI = 0;
    process->registersCpu->DI = 0;
    process->params = malloc(sizeof(paramsKernelForIO));
    process->params->isWaitingFs = false;
    process->params->listAux = list_create();
    return process;
};



//Agrega el proceso a la lista de NEW
void addPcbToNew(char* path)
{
    pcb_t *process = createProcess();

    sem_wait(&semPausePlanning);
    sem_post(&semPausePlanning);

    pthread_mutex_lock(&mutexSendProcessToMemory);
    sendProcessPathToMemory(process, path);
    sem_wait(&semMemoryOk); // Esperan a que la memoria de el ok de que el proceso se creo correctamente
    pthread_mutex_unlock(&mutexSendProcessToMemory);


    if(flagMemoryResponse)
    {
        list_push(pcbNewList, process); 
        //Log obligatorio
        log_info(getLogger(), "Se crea el proceso %d en NEW", process->pid);

        sem_post(&semNew);
    } else {

        log_info(getLogger(), "El proceso no se pudo abrir en memoria.");
        destroyProcess(process);

    }

    free(path);
    
    if(flagExecutingScript) pthread_mutex_unlock(&mutexOrderProcessByScript);
}

void destroyProcess(pcb_t *process)
{
    for(int i = 0; i < list_mutex_size(process->resources); i++) // Pasa por todos los recursos asignados que tiene para basicamente hacerles un signal.
    {
        resource_t* resourceToFree = list_pop(process->resources); // Popea el recurso asignado
        addInstanceResource(resourceToFree);
        
        if(list_mutex_size(resourceToFree->blockList) > 0){ // Se fija si tiene procesos bloqueados que esperen este recurso.
        pcb_t* processBlockToReady = list_pop(resourceToFree->blockList);
        list_push(processBlockToReady->resources, resourceToFree);
        list_remove_element_mutex(pcbBlockList, processBlockToReady);
        processBlockToReady->state = PCB_READY;
        list_push(pcbReadyList, processBlockToReady);
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_BLOCK - Estado Actual: PCB_READY", processBlockToReady->pid);

        sem_post(&semReady);
        }

    }

    for(int i = 0; i < list_mutex_size(resourcesBlockList); i++)
    {
        resource_t* resource = list_get(resourcesBlockList->list, i);

        bool flag = true;

        while(flag)
        {
            flag = list_remove_element_mutex(resource->blockList, process);
            if(flag) {
                addInstanceResource(resource);
                log_info(getLogger(), "Se libero el recurso que tenia %s, ahora tiene %d", resource->name, resource->instances);
            }
        }
    }

    for(int i = 0; i < list_mutex_size(interfacesList); i++)
    {
        interface_t *interface = list_get(interfacesList->list, i);

        bool flag = true;

        while(flag)
        {
            flag = list_remove_element_mutex(interface->blockList, process);
        }
                
    }

    for(int i = 0; i < list_size(process->params->listAux); i++)
    {
        physicalAddressInfoP *adresses = list_remove(process->params->listAux, 0);

        free(adresses->physicalAddress);
        free(adresses->size);
        free(adresses);
    }

    if(process->params->isWaitingFs){
        free(process->params->param3);
    }

    temporal_destroy(process->quantumForVRR);
    destroyListMutex(process->resources); 
    list_destroy(process->params->listAux);
    free(process->params);
    free(process->registersCpu);
    free(process);
}

void killProcess(uint32_t *paramkillProcessThread)
{    

    uint32_t pid = *paramkillProcessThread;

    pcb_t* processFound = foundStatePcb(pid);


    if(processFound == NULL){

        log_info(getLogger(), "PID: %d - No encontrado", pid);

        free(paramkillProcessThread);

        return;

    }

    processFound->processKilled = true;

    switch (processFound->state)
    {
    case PCB_NEW:

        list_remove_element_mutex(pcbNewList, processFound);

        processFound->state = PCB_EXIT;
        list_push(pcbExitList, processFound);

        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_NEW - Estado Actual: PCB_EXIT", processFound->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        processFound->state = PCB_NEW; // Este cambio devuelta es para que en exitState() pueda saber que era un proceso que no afectaba al grado de multiprogramacion.
        
        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INTERRUPTED_BY_USER", processFound->pid);
        sem_post(&semExit);

        break;
    
    case PCB_READY:

        list_remove_element_mutex(pcbReadyList, processFound);

        processFound->state = PCB_EXIT; 
        list_push(pcbExitList, processFound);

        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_READY - Estado Actual: PCB_EXIT", processFound->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INTERRUPTED_BY_USER", processFound->pid);

        sem_post(&semExit);


        break;

    case PCB_READY_PLUS:

        list_remove_element_mutex(pcbReadyPriorityList, processFound);

        processFound->state = PCB_EXIT; 
        list_push(pcbExitList, processFound);

        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_READY_PLUS - Estado Actual: PCB_EXIT", processFound->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INTERRUPTED_BY_USER", processFound->pid);

        sem_post(&semExit);


        break;

    case PCB_EXEC:

        sendInterruptForConsoleEndProcess(processFound);

        sem_wait(&semKillProcessExec);

        list_remove_element_mutex(pcbExecList, processFound);


        processFound->state = PCB_EXIT;

        list_push(pcbExitList, processFound);

        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processFound->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INTERRUPTED_BY_USER", processFound->pid);

        sem_post(&semExit);
        sem_post(&semMultiProcessing);       

 
        break;

    case PCB_BLOCK:


        if(processFound->isInInterface) // Esto es para el caso que se finalice un proceso y justo este operando algo en una interfaz, tiene que esperar hasta que termine para matarlo.
        { 
            interface_t* interface = foundInterfaceByProcessPidAssign(processFound);

            interface->flagKillProcess = true;

            sem_wait(&semKillProcessInInterface);

            pthread_mutex_lock(&mutexOrderPcbReadyPlus);


            if(interface == NULL) log_error(getLogger(), "Este error no deberia pasar nunca.");


            list_remove_element_mutex(pcbBlockList, processFound);

            processFound->state = PCB_EXIT;

            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_BLOCK - Estado Actual: PCB_EXIT", processFound->pid);

            sem_wait(&semPausePlanning);
            sem_post(&semPausePlanning);

            log_info(getLogger(), "Finaliza el proceso %d - Motivo: INTERRUPTED_BY_USER", processFound->pid);
            list_push(pcbExitList, processFound);
            sem_post(&semExit);

            pthread_mutex_unlock(&mutexOrderPcbReadyPlus);
        } else {

            list_remove_element_mutex(pcbBlockList, processFound);

            processFound->state = PCB_EXIT;

            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_BLOCK - Estado Actual: PCB_EXIT", processFound->pid);

            for(int i = 0; i < list_mutex_size(interfacesList); i++)
            {
                interface_t *interface = list_get(interfacesList->list, i);

                bool flag = true;

                while(flag)
                {
                    flag = list_remove_element_mutex(interface->blockList, processFound);
                }
                        
            }

            sem_wait(&semPausePlanning);
            sem_post(&semPausePlanning);

            log_info(getLogger(), "Finaliza el proceso %d - Motivo: INTERRUPTED_BY_USER", processFound->pid);
            list_push(pcbExitList, processFound);
            sem_post(&semExit);


        } 


        break;

    default:
        log_error(getLogger(), "Este error no deberia pasar nunca.");
        break;
    }

    free(paramkillProcessThread);


}

int pidToFind;

bool compare_pid(void *data) 
{
    pcb_t *pcb = (pcb_t *)data;
    return pcb->pid == pidToFind;
}

pcb_t* foundStatePcb(uint32_t pid)
{
    pidToFind = pid;

    pcb_t* processFound = (pcb_t *)list_find_mutex(pcbNewList, compare_pid);
    if(processFound != NULL) return processFound;
    
    processFound = (pcb_t *)list_find_mutex(pcbReadyList, compare_pid);
    if(processFound != NULL) return processFound;

    processFound = (pcb_t *)list_find_mutex(pcbReadyPriorityList, compare_pid);
    if(processFound != NULL) return processFound;

    processFound = (pcb_t *)list_find_mutex(pcbBlockList, compare_pid);
    if(processFound != NULL) return processFound;

    processFound = (pcb_t *)list_find_mutex(pcbExecList, compare_pid);
    if(processFound != NULL) return processFound;

    return NULL;
}
