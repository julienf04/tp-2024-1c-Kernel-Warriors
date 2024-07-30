#include "checkInterrupt.h"
#include "connections/clientCPU.h"



listMutex_t* interruptionsList;




void initInterrupts()
{
    interruptionsList = initListMutex();
}


void addInterruption(interruptionInfo* info)
{
    list_push(interruptionsList, info);
}


void checkInterrupts()
{
    sem_wait(&semCheckInterrupt);

    interruptionInfo* info;

    while (!list_is_empty_mutex(interruptionsList))
    {
        info = (interruptionInfo*)list_pop(interruptionsList);

        if (info->pid == getCurrentPID())
        {
            attendInterruption(info->type);
        }

        free(info);
    }

    sem_post(&semCheckInterrupt);
}

void attendInterruption(interruptionType type)
{
    switch (type)
    {
    case END_QUANTUM_TYPE:
        attendInterruptionEndQuantum();
        break;

    case END_PROCESS_TYPE:
        attendInterruptionEndProcess();
        break;
    }
}

void attendInterruptionEndQuantum()
{
    sendContextToKernel(CPU_SEND_CONTEXT_FOR_END_QUANTUM);
}

void attendInterruptionEndProcess()
{
    sendContextToKernel(CPU_SEND_CONTEXT_FOR_END_PROCESS);
}