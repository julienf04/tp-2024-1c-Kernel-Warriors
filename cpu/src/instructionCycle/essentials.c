#include "essentials.h"



int PID;



sem_t semaphoreWaitInstruction;

sem_t semCheckInterrupt;

sem_t semContinueInstructionCycle;

sem_t semWaitDataFromMemory;

sem_t semWaitConfirmationFromMemory;





void setCurrentPID(int value)
{
    PID = value;
}

int getCurrentPID()
{
    return PID;
}


void getCurrentContextProcess(contextProcess* context)
{
    context->pc = getPC();
    context->registersCpu.AX = getAX();
    context->registersCpu.BX = getBX();
    context->registersCpu.CX = getCX();
    context->registersCpu.DX = getDX();
    context->registersCpu.EAX = getEAX();
    context->registersCpu.EBX = getEBX();
    context->registersCpu.ECX = getECX();
    context->registersCpu.EDX = getEDX();
    context->registersCpu.DI = getDI();
    context->registersCpu.SI = getSI();
}