#include "serverCPU.h"
#include "utils/server/utils.h"
#include "utilsCPU/logger.h"
#include "utilsCPU/config.h"
#include "utils/utilsGeneral.h"
#include "instructionCycle/fetch.h"
#include "MMU/MMU.h"
#include "MMU/TLB.h"
#include "instructionCycle/checkInterrupt.h"

int numberOfKernelClientsForDispatch = 0;
int numberOfKernelClientsForInterrupt = 0;

sem_t semaphoreForKernelDispatch;
sem_t semaphoreForKernelInterrupt;

bool _finishAllServersSignal = false;


int socketMemory;
int socketKernelDispatch;
int socketKernelInterrupt;



void receiveClientIterationDispatch(int socketServer)
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
    case KERNEL_MODULE_TO_CPU_DISPATCH:

        if (numberOfKernelClientsForDispatch >= MAX_KERNEL_CLIENTS_FOR_DISPATCH)
        {
            log_info(getLogger(), "Un cliente Kernel se intento conectar en un servidor para el Dispatch. Fue rechazado debido a que se alcanzo la cantidad maxima de clientes.");
            break;
        }

        log_info(getLogger(), "Se conectó un módulo Kernel en el Dispatch");

        initServerForASocket(socketClient, serverCPUDispatchForKernel);

        sem_wait(&semaphoreForKernelDispatch);
        numberOfKernelClientsForDispatch++;
        sem_post(&semaphoreForKernelDispatch);

        break;

    case DO_NOTHING:
        break;


    case ERROR:
        log_error(getLogger(), ERROR_CASE_MESSAGE);
        break;

    default:
        log_error(getLogger(), DEFAULT_CASE_MESSAGE);
        exit(EXIT_FAILURE);
    }
}

void receiveClientIterationInterrupt(int socketServer)
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
    case KERNEL_MODULE_TO_CPU_INTERRUPT:

        if (numberOfKernelClientsForInterrupt >= MAX_KERNEL_CLIENTS_FOR_INTERRUPT)
        {
            log_info(getLogger(), "Un cliente Kernel se intento conectar en un servidor para el Interrupt. Fue rechazado debido a que se alcanzo la cantidad maxima de clientes.");
            break;
        }

        log_info(getLogger(), "Se conectó un módulo Kernel en el Interrupt");

        initServerForASocket(socketClient, serverCPUInterruptForKernel);

        sem_wait(&semaphoreForKernelInterrupt);
        numberOfKernelClientsForInterrupt++;
        sem_post(&semaphoreForKernelInterrupt);

        break;

    case DO_NOTHING:
        break;


    case ERROR:
        log_error(getLogger(), ERROR_CASE_MESSAGE);
        break;

    default:
        log_error(getLogger(), DEFAULT_CASE_MESSAGE);
        exit(EXIT_FAILURE);
    }
}

void serverCPUDispatchForKernel(int *socketClient)
{
    socketKernelDispatch = *socketClient;

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
        case KERNEL_SEND_CONTEXT:
            receiveContext(socketClient);
            break;

        case DO_NOTHING:
            break;


        case ERROR:
            log_error(getLogger(), ERROR_CASE_MESSAGE);
            exitLoop = true;
            break;

        default:
            log_error(getLogger(), DEFAULT_CASE_MESSAGE);
            exit(EXIT_FAILURE);
        }
    }

    free(socketClient);

    sem_wait(&semaphoreForKernelDispatch);
    numberOfKernelClientsForDispatch--;
    sem_post(&semaphoreForKernelDispatch);
}

void serverCPUInterruptForKernel(int *socketClient)
{
    socketKernelInterrupt = *socketClient;

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
        case KERNEL_SEND_INTERRUPT_QUANTUM_END:
            kernelInterruptEndQuantum(socketClient);
            break;

        case KERNEL_SEND_INTERRUPT_CONSOLE_END_PROCESS:
            kernelInterruptEndProcess(socketClient);
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

    sem_wait(&semaphoreForKernelInterrupt);
    numberOfKernelClientsForInterrupt--;
    sem_post(&semaphoreForKernelInterrupt);
}

void serverCPUForMemory(int *socketClient)
{
    socketMemory = *socketClient;

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
        case MEMORY_NEXT_INSTRUCTION:
            memoryNextInstruction(socketClient);
            break;

        case MEMORY_TAM_PAGINA:
            memoryTamPaginaRecive(socketClient);
            break;

        case MEMORY_SEND_FRAME:
            memoryGetFrame(socketClient);
            break;

        case MEMORY_SEND_DATA:
            memoryReceiveData(socketClient);
            break;

        case MEMORY_RESIZE_OK:
            memoryReceiveConfirmationForResize(socketClient);
            break;

        case MEMORY_OUT_OF_MEMORY:
            memoryReceiveOutOfMemory(socketClient);
            break;

        case MEMORY_WRITE_OK:
            memoryReceiveConfirmationForWrite(socketClient);
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
}



void memoryNextInstruction(int* socketClient)
{
    // Recibo el mensaje por parte de la memoria, lo almaceno en el lugar correspondiente y destruyo la lista (aun hay que liberar la memoria del string, pero eso es responsabilidad de quien lo use)
    t_list *listPackage = getPackage(*socketClient);

    currentInstructionString = malloc(sizeof(instructionString));
    currentInstructionString->string = (char*)list_get(listPackage, 0); // Obtengo la instruccion en forma de string

    sem_post(&semaphoreWaitInstruction); // Le aviso a la fase fetch del ciclo de instruccion que ya se obtuvo la instruccion en forma de string

    list_destroy(listPackage);
}

void memoryTamPaginaRecive(int* socketClient)
{
    // Recibo el mensaje por parte de la memoria, lo almaceno en el lugar correspondiente y destruyo la lista.
    t_list *listPackage = getPackage(*socketClient);

    memoryTamPagina tamPagina;
    tamPagina.tamPagina = *((int*)list_get(listPackage, 0)); // Obtengo el tamaño de pagina

    setTamPagina(tamPagina.tamPagina);

    list_destroy_and_destroy_elements(listPackage, free);
}

void memoryGetFrame(int* socketClient)
{
    // Recibo el mensaje por parte de la memoria, lo almaceno en el lugar correspondiente y destruyo la lista.
    t_list *listPackage = getPackage(*socketClient);

    sendFrameInfo frameInfo;
    frameInfo.frame = *((int*)list_get(listPackage, 0)); // Obtengo el frame

    setCurrentFrame(frameInfo.frame);

    sem_post(&semTLBMiss); // Le aviso a la TLB que ya le llegó el frame de la Memoria.


    list_destroy_and_destroy_elements(listPackage, free);
}



void kernelInterruptEndQuantum(int* socketClient)
{
    // Recibo el mensaje por parte de la memoria, lo almaceno en el lugar correspondiente y destruyo la lista.
    t_list *listPackage = getPackage(*socketClient);

    int pid;
    pid = *((int*)list_get(listPackage, 0));


    sem_wait(&semCheckInterrupt);

    interruptionInfo* info = malloc(sizeof(interruptionInfo));
    info->pid = pid;
    info->type = END_QUANTUM_TYPE;
    addInterruption(info);

    sem_post(&semCheckInterrupt);



    list_destroy_and_destroy_elements(listPackage, free);
}


void kernelInterruptEndProcess(int* socketClient)
{
    // Recibo el mensaje por parte de la memoria, lo almaceno en el lugar correspondiente y destruyo la lista.
    t_list *listPackage = getPackage(*socketClient);

    int pid;
    pid = *((int*)list_get(listPackage, 0));


    sem_wait(&semCheckInterrupt);

    interruptionInfo* info = malloc(sizeof(interruptionInfo));
    info->pid = pid;
    info->type = END_PROCESS_TYPE;
    addInterruption(info);

    sem_post(&semCheckInterrupt);



    list_destroy_and_destroy_elements(listPackage, free);
}


void receiveContext(int* socketClient)
{
    // Recibo el mensaje por parte de la memoria, lo almaceno en el lugar correspondiente y destruyo la lista.
    t_list *listPackage = getPackage(*socketClient);

    contextProcess contextProcess;
    
    // Recibe todo el contexto del proceso ejecutando de CPU
    contextProcess.pc = *(uint32_t *)list_get(listPackage, 0);
    contextProcess.registersCpu.AX = *(uint8_t *)list_get(listPackage, 1);
    contextProcess.registersCpu.BX = *(uint8_t *)list_get(listPackage, 2);
    contextProcess.registersCpu.CX = *(uint8_t *)list_get(listPackage, 3);
    contextProcess.registersCpu.DX = *(uint8_t *)list_get(listPackage, 4);
    contextProcess.registersCpu.EAX = *(uint32_t *)list_get(listPackage, 5);
    contextProcess.registersCpu.EBX = *(uint32_t *)list_get(listPackage, 6);
    contextProcess.registersCpu.ECX = *(uint32_t *)list_get(listPackage, 7);
    contextProcess.registersCpu.EDX = *(uint32_t *)list_get(listPackage, 8);
    contextProcess.registersCpu.DI = *(uint32_t *)list_get(listPackage, 9);
    contextProcess.registersCpu.SI = *(uint32_t *)list_get(listPackage, 10);

    int pid = *(uint32_t *)list_get(listPackage, 11);

    setPC(contextProcess.pc);
    setAX(contextProcess.registersCpu.AX);
    setBX(contextProcess.registersCpu.BX);
    setCX(contextProcess.registersCpu.CX);
    setDX(contextProcess.registersCpu.DX);
    setEAX(contextProcess.registersCpu.EAX);
    setEBX(contextProcess.registersCpu.EBX);
    setECX(contextProcess.registersCpu.ECX);
    setEDX(contextProcess.registersCpu.EDX);
    setDI(contextProcess.registersCpu.DI);
    setSI(contextProcess.registersCpu.SI);


    setCurrentPID(pid);

    sem_post(&semContinueInstructionCycle);



    list_destroy_and_destroy_elements(listPackage, free);
}

void memoryReceiveData(int* socketClient)
{
    // Recibo el mensaje por parte de la memoria, lo almaceno en el lugar correspondiente y destruyo la lista.
    t_list *listPackage = getPackage(*socketClient);

    dataReceivedFromMemory = list_get(listPackage, 0);

    sem_post(&semWaitDataFromMemory);

    list_destroy(listPackage);
}


void memoryReceiveConfirmationForWrite(int* socketClient)
{
    sem_post(&semWaitConfirmationFromMemory);
}

void memoryReceiveConfirmationForResize(int* socketClient)
{
    resizeResultReceivedFromMemory = RESIZE_SUCCESS;

    sem_post(&semWaitConfirmationFromMemory);
}

void memoryReceiveOutOfMemory(int* socketClient)
{
    resizeResultReceivedFromMemory = OUT_OF_MEMORY;

    sem_post(&semWaitConfirmationFromMemory);
}


void finishAllServersSignal()
{
    _finishAllServersSignal = true;
}