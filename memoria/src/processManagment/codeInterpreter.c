#include "codeInterpreter.h"
#include "utilsMemory/delay.h"
#include <stdio.h>
#include <stdlib.h>






char* getInstruction(int PID, int PC)
{
    memoryDelay();

    processInfo* currentPseudocode = getLastProcess();

    if (currentPseudocode == NULL || currentPseudocode->PID != PID)
    {
        sem_wait(&semAuxPID);
        auxPID = PID;
        currentPseudocode = list_find_mutex(processesList, closurePIDsAreEqual);
        sem_post(&semAuxPID);
        if (currentPseudocode == NULL)
        {
            exit(EXIT_FAILURE);
        }
        setLastProcess(currentPseudocode);
    }

    return currentPseudocode->pseudocodeInstructions[PC];
}


