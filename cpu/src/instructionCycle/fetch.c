#include "fetch.h"
#include "utils/client/utils.h"
#include "connections/clientCPU.h"
#include "utilsCPU/logger.h"


instructionString* currentInstructionString;


instructionString* getNextInstruction(int PID, uint32_t PC)
{

    // Envio la operacion y el Program Counter a la memoria para avisarle que me tiene que dar la proxima instruccion
    sendPCToMemory(PID, PC);

    logFetchInstruction(PID, PC);

    incrementPC();

    // Espero a que la memoria me responda con la proxima instruccion (El servidor de la CPU para la Memoria va a hacer un signal cuando ya haya recibido la instruccion)
    sem_wait(&semaphoreWaitInstruction);

    
    return currentInstructionString;
}
