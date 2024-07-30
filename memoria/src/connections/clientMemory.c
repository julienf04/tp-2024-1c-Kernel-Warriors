#include "clientMemory.h"
#include "processManagment/codeInterpreter.h"
#include "serverMemory.h"
#include "utilsMemory/delay.h"



void sendTamPagina(int socketClient, int tamPagina)
{
    // Envio el tamaño de pagina a la CPU.
    t_package* package = createPackage(MEMORY_TAM_PAGINA);

    memoryTamPagina tamPaginaStruct;
    tamPaginaStruct.tamPagina = getMemoryConfig()->TAM_PAGINA;

    addToPackage(package, &(tamPaginaStruct.tamPagina), sizeof(int)); // Agrego la info al paquete

    sendPackage(package, socketClient); // Envio el paquete

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)
}


void sendInstructionToCpu(int socketClient, cpuGiveMeNextInstruction* params)
{
    // Envio el string con la instruccion.
    t_package* package = createPackage(MEMORY_NEXT_INSTRUCTION);

    instructionString newInstruction;
    newInstruction.string = getInstruction(params->PID, params->PC);

    addToPackage(package, newInstruction.string, string_length(newInstruction.string) + 1); // Agrego el string de la instruccion (+ 1 en el length por el caracter nulo \0)

    sendPackage(package, socketClient); // Envio el paquete

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)
}



void sendData(int socketClient, void* data, int size)
{
    t_package* package = createPackage(MEMORY_SEND_DATA);

    sendDataInfo info;
    info.data = data;
    info.size = size;

    addToPackage(package, info.data, info.size); // Agrego la data al paquete

    sendPackage(package, socketClient); // Envio la data

    destroyPackage(package);
}



void sendConfirmation(int socketClient, operationCode confirmationCode)
{
    t_package* package = createPackage(confirmationCode);

    sendPackage(package, socketClient); // Envio la confirmacion

    destroyPackage(package);
}


void sendProcessCreatedResult(int socketClient, bool result)
{
    t_package* package = createPackage(MEMORY_SEND_RESPONSE_FOR_NEW_PROCESS);

    addToPackage(package, &result, sizeof(bool)); // Agrego la info al paquete

    sendPackage(package, socketClient); // Envio la confirmacion

    destroyPackage(package);
}


void sendFrame(int socketClient, int frame)
{
    // Retardo de la operacion
    memoryDelay();

    // La operacion
    t_package* package = createPackage(MEMORY_SEND_FRAME);

    sendFrameInfo info;
    info.frame = frame;

    addToPackage(package, &(info.frame), sizeof(int)); // Agrego la data al paquete

    sendPackage(package, socketClient); // Envio la data

    destroyPackage(package);
}