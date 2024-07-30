#include "instructionCycle.h"
#include "cpuDebug.h"


#ifdef DEBUG_CPU
#include "utilsCPU/debugUtils.h"
#endif

void runInstructionCycle()
{
    initInterrupts();

    while (true)
    {
        // Si no hay ningun proceso para ejecutar, se queda esperando a que llegue uno y le avisen, o tambien espera si hay otro hilo que tiene que ejecutar algo antes
        sem_wait(&semContinueInstructionCycle);
        
        
        // Fase fetch, busco la siguiente instruccion
        instructionString* newInstructionString = getNextInstruction(getCurrentPID(), getPC());

        // Fase decode, descifro qué me pide la instrucción (la cual llegó en forma de string)
        void** instructionParams = malloc(sizeof(void**));
        *instructionParams = NULL;
        instructionType type = decodeInstruction(newInstructionString->string, instructionParams);

        // Ejecuto la instruccion
        executeInstruction(type, *instructionParams);

        #ifdef DEBUG_CPU
        printRegisters();
        #endif


        // Libero la memoria que usé
        if (*instructionParams != NULL)
        {
            free(*instructionParams);
        }
        free(instructionParams);
        free(newInstructionString->string);
        free(newInstructionString);

        // Me fijo si tengo interrupciones
        checkInterrupts();


        // Ya terminó el ciclo de instruccion, puede pero se hace un post a si mismo para seguir el bucle
        sem_post(&semContinueInstructionCycle);

    }
    
}