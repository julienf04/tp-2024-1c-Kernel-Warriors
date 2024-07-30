#include "clientCPU.h"
#include "instructionCycle/essentials.h"



void sendPCToMemory(int PID, uint32_t PC)
{
    // Envio la operacion y el Program Counter a la memoria para avisarle que me tiene que dar la proxima instruccion
    t_package* package = createPackage(CPU_GIVE_ME_NEXT_INSTRUCTION);

    cpuGiveMeNextInstruction giveMeNextInstruction;
    giveMeNextInstruction.PID = PID;
    giveMeNextInstruction.PC = PC;

    addToPackage(package, &(giveMeNextInstruction.PID), sizeof(int)); // Agrego el Process ID al paquete para enviar
    addToPackage(package, &(giveMeNextInstruction.PC), sizeof(uint32_t)); // Agrego el Program Counter al paquete para enviar

    sendPackage(package, socketMemory); // Envio el Program Counter

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)
}

void requestFrame(int pid, int page)
{
    // Envio la operacion y la info para pedirle el frame que necesito a la Memoria
    t_package* package = createPackage(CPU_GET_FRAME);

    getFrameInfo frameInfo;
    frameInfo.PID = pid;
    frameInfo.page = page;

    addToPackage(package, &(frameInfo.PID), sizeof(int)); // Agrego el Process ID al paquete para enviar
    addToPackage(package, &(frameInfo.page), sizeof(int)); // Agrego el Program Counter al paquete para enviar

    sendPackage(package, socketMemory); // Envio el Program Counter

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)
}

void sendResizeMemory(int pid, int bytes)
{
    // Envio la operacion y la info para pedirle el frame que necesito a la Memoria
    t_package* package = createPackage(CPU_RESIZE_MEMORY);

    requestResizeMemoryInfo resizeInfo;
    resizeInfo.pid = pid;
    resizeInfo.bytes = bytes;

    addToPackage(package, &(resizeInfo.pid), sizeof(int)); // Agrego el Process ID al paquete para enviar
    addToPackage(package, &(resizeInfo.bytes), sizeof(int)); // Agrego los bytes al paquete

    sendPackage(package, socketMemory); // Envio el Program Counter

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)
}


void sendReadMemory(int pid, int physicalAddress, int size)
{
    // Envio la operacion y la info para pedirle el frame que necesito a la Memoria
    t_package* package = createPackage(READ_MEMORY);

    requestReadMemoryInfo readInfo;
    readInfo.pid = pid;
    readInfo.physicalAddress = physicalAddress;
    readInfo.size = size;

    addToPackage(package, &(readInfo.pid), sizeof(int)); // Agrego el Process ID al paquete para enviar
    addToPackage(package, &(readInfo.physicalAddress), sizeof(int)); // Agrego la direccion fisica.
    addToPackage(package, &(readInfo.size), sizeof(int)); // Agrego el tamaño a leer.

    sendPackage(package, socketMemory); // Envio el Program Counter

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)
}

void sendWriteMemory(int pid, void* data, int physicalAddress, int size)
{
    // Envio la operacion y la info para pedirle el frame que necesito a la Memoria
    t_package* package = createPackage(WRITE_MEMORY);

    requestWriteMemoryInfo writeInfo;
    writeInfo.pid = pid;
    writeInfo.data = data;
    writeInfo.physicalAddress = physicalAddress;
    writeInfo.size = size;

    addToPackage(package, &(writeInfo.pid), sizeof(int)); // Agrego el Process ID al paquete para enviar
    addToPackage(package, writeInfo.data, size); // Agrego el Process ID al paquete para enviar
    addToPackage(package, &(writeInfo.physicalAddress), sizeof(int)); // Agrego el Process ID al paquete para enviar
    addToPackage(package, &(writeInfo.size), sizeof(int)); // Agrego el Process ID al paquete para enviar

    sendPackage(package, socketMemory); // Envio el Program Counter

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)
}


void sendContextToKernel(operationCode opCode)
{
    // Envio la operacion y el Program Counter a la memoria para avisarle que me tiene que dar la proxima instruccion
    t_package* package = createPackage(opCode);

    contextProcess contextProcess;
    getCurrentContextProcess(&contextProcess);

    addContextToPackage(package, &contextProcess);

    sendPackage(package, socketKernelDispatch); // Envio el Contexto

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)

    sem_wait(&semContinueInstructionCycle);
}


void sendContextToKernelForResource(operationCode opCode, char* resource)
{
    // Envio la operacion y el Program Counter a la memoria para avisarle que me tiene que dar la proxima instruccion
    t_package* package = createPackage(opCode);

    contextProcess contextProcess;
    getCurrentContextProcess(&contextProcess);

    addContextToPackage(package, &contextProcess);
    addToPackage(package, resource, string_length(resource) + 1); // + 1 por el caracter nulo \0

    sendPackage(package, socketKernelDispatch); // Envio el Contexto

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)

    sem_wait(&semContinueInstructionCycle);
}


void sendContextToKernelForIOGeneric(char* nameInterface, uint32_t workUnits)
{
    // Envio la operacion y el Program Counter a la memoria para avisarle que me tiene que dar la proxima instruccion
    t_package* package = createPackage(CPU_SEND_CONTEXT_FOR_IO_GENERIC);

    contextProcess contextProcess;
    getCurrentContextProcess(&contextProcess);

    addContextToPackage(package, &contextProcess);
    addToPackage(package, nameInterface, string_length(nameInterface) + 1); // + 1 por el caracter nulo \0
    addToPackage(package, &workUnits, sizeof(uint32_t));

    sendPackage(package, socketKernelDispatch); // Envio el Contexto

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)

    sem_wait(&semContinueInstructionCycle);
}


void sendContextToKernelForIOReadOrWrite(operationCode opCode, char* nameInterface, int amountOfPhysicalAddresses, physicalAddressInfo* physicalAddressesArray, int sizeToReadOrWrite)
{
    // Envio la operacion y el Program Counter a la memoria para avisarle que me tiene que dar la proxima instruccion
    t_package* package = createPackage(opCode);

    contextProcess contextProcess;
    getCurrentContextProcess(&contextProcess);

    addContextToPackage(package, &contextProcess);
    addToPackage(package, nameInterface, string_length(nameInterface) + 1); // + 1 por el caracter nulo \0

    addToPackage(package, &amountOfPhysicalAddresses, sizeof(int));
    for (int i = 0; i < amountOfPhysicalAddresses; i++)
    {
        addToPackage(package, &(physicalAddressesArray[i].physicalAddress), sizeof(int));
        addToPackage(package, &(physicalAddressesArray[i].size), sizeof(int));
    }

    addToPackage(package, &sizeToReadOrWrite, sizeof(int));
    

    sendPackage(package, socketKernelDispatch); // Envio el Contexto

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)

    sem_wait(&semContinueInstructionCycle);
}


void sendContextToKernelForIOFSCreateOrDelete(operationCode opCode, char* nameInterface, char* fileName)
{
    t_package* package = createPackage(opCode);

    contextProcess contextProcess;
    getCurrentContextProcess(&contextProcess);

    addContextToPackage(package, &contextProcess);

    addToPackage(package, nameInterface, string_length(nameInterface) + 1); // + 1 por el caracter nulo \0
    addToPackage(package, fileName, string_length(fileName) + 1); // + 1 por el caracter nulo \0

    sendPackage(package, socketKernelDispatch); // Envio el Contexto

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)

    sem_wait(&semContinueInstructionCycle);
}


void sendContextToKernelForIOFSTruncate(char* nameInterface, char* fileName, uint32_t size)
{
    t_package* package = createPackage(CPU_SEND_CONTEXT_FOR_IO_FS_TRUNCATE);

    contextProcess contextProcess;
    getCurrentContextProcess(&contextProcess);

    addContextToPackage(package, &contextProcess);

    addToPackage(package, nameInterface, string_length(nameInterface) + 1); // + 1 por el caracter nulo \0
    addToPackage(package, fileName, string_length(fileName) + 1); // + 1 por el caracter nulo \0
    addToPackage(package, &size, sizeof(uint32_t));

    sendPackage(package, socketKernelDispatch); // Envio el Contexto

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)

    sem_wait(&semContinueInstructionCycle);
}


void sendContextToKernelForIOFSReadOrWrite(operationCode opCode, char* nameInterface, char* fileName, int amountOfPhysicalAddresses, physicalAddressInfo* physicalAddressesArray, int sizeToReadOrWrite, int filePointer)
{
    t_package* package = createPackage(opCode);

    contextProcess contextProcess;
    getCurrentContextProcess(&contextProcess);

    addContextToPackage(package, &contextProcess);

    addToPackage(package, nameInterface, string_length(nameInterface) + 1); // + 1 por el caracter nulo \0
    addToPackage(package, fileName, string_length(fileName) + 1); // + 1 por el caracter nulo \0

    addToPackage(package, &amountOfPhysicalAddresses, sizeof(int));
    for (int i = 0; i < amountOfPhysicalAddresses; i++)
    {
        addToPackage(package, &(physicalAddressesArray[i].physicalAddress), sizeof(int));
        addToPackage(package, &(physicalAddressesArray[i].size), sizeof(int));
    }

    addToPackage(package, &sizeToReadOrWrite, sizeof(int));
    addToPackage(package, &filePointer, sizeof(int));

    sendPackage(package, socketKernelDispatch); // Envio el Contexto

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)

    sem_wait(&semContinueInstructionCycle);
}




/////////////////// FUNCIONES AUXILIARES ///////////////////


void addContextToPackage(t_package* package, contextProcess* context)
{
    addToPackage(package, &(context->pc), sizeof(uint32_t)); // Agrego el Program Counter del registros al paquete para enviar
    addToPackage(package, &(context->registersCpu.AX), sizeof(uint8_t)); // Agrego los registros del contexto
    addToPackage(package, &(context->registersCpu.BX), sizeof(uint8_t)); // Agrego los registros del contexto
    addToPackage(package, &(context->registersCpu.CX), sizeof(uint8_t)); // Agrego los registros del contexto
    addToPackage(package, &(context->registersCpu.DX), sizeof(uint8_t)); // Agrego los registros del contexto
    addToPackage(package, &(context->registersCpu.EAX), sizeof(uint32_t)); // Agrego los registros del contexto
    addToPackage(package, &(context->registersCpu.EBX), sizeof(uint32_t)); // Agrego los registros del contexto
    addToPackage(package, &(context->registersCpu.ECX), sizeof(uint32_t)); // Agrego los registros del contexto
    addToPackage(package, &(context->registersCpu.EDX), sizeof(uint32_t)); // Agrego los registros del contexto
    addToPackage(package, &(context->registersCpu.DI), sizeof(uint32_t)); // Agrego los registros del contexto
    addToPackage(package, &(context->registersCpu.SI), sizeof(uint32_t)); // Agrego los registros del contexto

}