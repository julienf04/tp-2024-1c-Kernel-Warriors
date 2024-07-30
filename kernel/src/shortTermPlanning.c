#include "shortTermPlanning.h"

void readyState()
{
    
    while (1)
    {

        sem_wait(&semReady);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        char* listPids = _listPids(pcbReadyList->list);
        //Log Obligatorio
        log_info(getLogger(), listPids, "Ready" ,getKernelConfig()->ALGORITMO_PLANIFICACION);

        free(listPids);
    
        sem_post(&semExec);

    }
}


void execState()
{
    while(1)
    {

        sem_wait(&semMultiProcessing); // Espero que se desocupe la CPU
        sem_wait(&semExec); 

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        pcb_t *pcbToExec;
        bool flagAuxVRR = false; // Utilizo esto para saber si el proceso estaba anteriormente en pcbReadyPriorityList o no
        
        if(!list_mutex_is_empty(pcbReadyList) || !list_mutex_is_empty(pcbReadyPriorityList))
        {
            if(algorithm != VRR){ // FIFO Y RR tienen practicamente el mismo comportamiento. 

                pcbToExec = list_pop(pcbReadyList);
                list_push(pcbExecList, pcbToExec);
                pcbToExec->state = PCB_EXEC;

                log_info(getLogger(), "PID: %d - Estado Anterior: PCB_READY - Estado Actual: PCB_EXEC", pcbToExec->pid);

            } else {

                if(list_mutex_is_empty(pcbReadyPriorityList)){ // Si el algoritmo de planificacion es VRR y pcbReadyPriorityList esta vacia, tiene el mismo comportamiento que RR o FIFO.
                    
                    pcbToExec = list_pop(pcbReadyList);
                    list_push(pcbExecList, pcbToExec);
                    pcbToExec->state = PCB_EXEC;

                    log_info(getLogger(), "PID: %d - Estado Anterior: PCB_READY - Estado Actual: PCB_EXEC", pcbToExec->pid);

                } else {

                    pcbToExec = list_pop(pcbReadyPriorityList);
                    list_push(pcbExecList, pcbToExec);
                    pcbToExec->state = PCB_EXEC;
                    log_info(getLogger(), "PID: %d - Estado Anterior: PCB_READY_PLUS - Estado Actual: PCB_EXEC", pcbToExec->pid);
                    flagAuxVRR = true;

                }
            }

            switch (algorithm)
            {
            case FIFO:
                //No hace nada porque es el caso default
                sendContextToCPU(pcbToExec);
                break;
            
            case RR:
                sendContextToCPU(pcbToExec);
                pthread_t QuantumInterruptThread;
                pthread_create(&QuantumInterruptThread, NULL, (void*)quantumControlInterrupt, pcbToExec);
                pthread_detach(QuantumInterruptThread);
                break;

            case VRR:
                if(flagAuxVRR){ //Quiere decir que estaba en pcbReadyPriorityList, entonces me aseguro que tiene algo de quantum sobrante.

                    paramsQuantumVRRThread *paramsQuantumVRR = malloc(sizeof(paramsQuantumVRRThread));
                    paramsQuantumVRR->process = pcbToExec;
                    paramsQuantumVRR->timeForQuantum = getKernelConfig()->QUANTUM - temporal_gettime(pcbToExec->quantumForVRR); // El tiempo del Quantum - el tiempo que estuve en exec. Ese es el tiempo que va a estar en exec por estar en pcbReadyPriorityList.

                    sendContextToCPU(pcbToExec);

                    pthread_t QuantumInterruptThread;
                    pthread_create(&QuantumInterruptThread, NULL, (void*)quantumControlInterruptVRR, paramsQuantumVRR);
                    pthread_detach(QuantumInterruptThread);

                } else { // Como no hay nada en pcbReadyPriorityList, actua como RR pero ademas contando el tiempo.

                    
                    //pcbToExec->quantumForVRR = temporal_create();

                    pcbToExec->quantumForVRR->elapsed_ms = 0;
                    clock_gettime(CLOCK_MONOTONIC_RAW, &(pcbToExec->quantumForVRR->current));
                    temporal_resume(pcbToExec->quantumForVRR);  /// Antes del send inicia el tiempo. A tener en cuenta la diferencia de ms, puede ser que se tenga que iniciar el temporizador despues de que se mande.
                    sendContextToCPU(pcbToExec);

                    pthread_t QuantumInterruptThread;
                    pthread_create(&QuantumInterruptThread, NULL, (void*)quantumControlInterrupt, pcbToExec);
                    pthread_detach(QuantumInterruptThread);

                }
                break;

            default:
                break;
            }
            
        } else { // Caso muy especifico para solucionar un bug.

            sem_post(&semMultiProcessing);

        }

    }
  
}

void blockState()
{
    while (1)
    {
        sem_wait(&semBlock);
    }
    
}

void initShortTermPlanning()
{
    pthread_t readyStateThread;
    pthread_t execStateThread;
    pthread_t blockStateThread;
    pthread_create(&readyStateThread, NULL, (void*)readyState, NULL);
    pthread_create(&execStateThread, NULL, (void*)execState, NULL);
    pthread_create(&blockStateThread, NULL, (void*)blockState, NULL);
    pthread_detach(readyStateThread);
    pthread_detach(execStateThread);
    pthread_detach(blockStateThread);
}

char* _listPids(t_list *list)
{
    char* pids = string_new();
    string_append(&pids, "Cola %s %s: [");

    for (int i = 0; i < list_size(list); i++)
    {
        pcb_t *process = list_get(list, i);
        char *pid = string_from_format("%d", process->pid);
        string_append(&pids, " ");
        string_append(&pids, pid);

        free(pid);
    }
    string_append(&pids, " ]");

    return pids;
}

void quantumControlInterrupt(pcb_t* pcbToExec)
{
        // Bloquea el hilo durante el quantum de tiempo
        usleep(getKernelConfig()->QUANTUM*1000);

        // Verifica si el proceso sigue ejecutándose o si termino el proceso y se elimino
        if (pcbToExec->state == PCB_EXEC) {
            // Enviar interrupción para desalojar el proceso
            sendInterruptForQuantumEnd(pcbToExec);
        }
}

void quantumControlInterruptVRR(paramsQuantumVRRThread* paramsQuantumVRRThread)
{
    // Bloquea el hilo durante el quantum de tiempo
    pcb_t *pcbToExec = paramsQuantumVRRThread->process;
    int64_t timeQuantum = paramsQuantumVRRThread->timeForQuantum;

    usleep(timeQuantum*1000);

    free(paramsQuantumVRRThread);

    // Verifica si el proceso sigue ejecutándose o si termino el proceso y se elimino
    if (pcbToExec->state == PCB_EXEC) {
        // Enviar interrupción para desalojar el proceso
        sendInterruptForQuantumEnd(pcbToExec);
    }

}

void defineAlgorithm()
{
    if(string_equals_ignore_case(getKernelConfig()->ALGORITMO_PLANIFICACION, "FIFO")) algorithm = FIFO;
    else if(string_equals_ignore_case(getKernelConfig()->ALGORITMO_PLANIFICACION, "RR")) algorithm = RR;
    else if(string_equals_ignore_case(getKernelConfig()->ALGORITMO_PLANIFICACION, "VRR")) algorithm = VRR;
}