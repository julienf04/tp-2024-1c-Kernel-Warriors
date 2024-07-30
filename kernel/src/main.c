#include "kernel.h"
#include "utils/config.h"

int main()
{
    // Inicio el logger general del modulo. Siempre deberia ser la primera sentencia a ejecutar del main.
    initLogger("kernel.log", "kernel", true, LOG_LEVEL_INFO);

    // Obtengo la configuracion general del kernel.
    char* path = askForConfigPath();
    initKernelConfig(path);
    free(path);

    // Inicializo todo.
    pid = 0;

    diffBetweenNewAndPrevMultiprogramming = 0;

    pthread_mutex_init(&mutexSendProcessToMemory, NULL);
    pthread_mutex_init(&mutexOrderProcessByScript, NULL);
    pthread_mutex_init(&mutexOrderReadyExecProcess, NULL);
    pthread_mutex_init(&mutexOrderPcbReadyPlus, NULL);

    sem_init(&semNew, 0, 0);
    sem_init(&semReady, 0, 0);
    sem_init(&semExec, 0, 0);
    sem_init(&semBlock, 0, 0);
    sem_init(&semExit, 0, 0);
    sem_init(&semAddPid, 0, 1);
    sem_init(&semMultiProcessing, 0 , 1);
    sem_init(&semaphoreForIO, 0, 1); 
    sem_init(&semMultiProgramming, 0 , getKernelConfig()->GRADO_MULTIPROGRAMACION);
    sem_init(&semPausePlanning, 0, 1);
    sem_init(&semMemoryOk, 0, 0);
    sem_init(&semKillProcessInInterface, 0, 0);
    sem_init(&semKillProcessExec, 0, 0);

    defineAlgorithm();

    pcbNewList = initListMutex();
    pcbReadyList = initListMutex();
    pcbExecList = initListMutex();
    pcbBlockList = initListMutex();
    pcbExitList = initListMutex();
    pcbReadyPriorityList = initListMutex();

    resourcesBlockList = initListMutex();

    interfacesList = initListMutex();

    initResources();

    initLongTermPlanning();
    initShortTermPlanning();

    // Creo y pongo a correr el/los threads de el/los servidores de este modulo
    waitClientsLoopParams params;
    params.logger = getLogger();
    params.portToListen = getKernelConfig()->PUERTO_ESCUCHA;
    params.eachIterationFunc = receiveClientIteration;
    params.finishLoopSignal = &_finishAllServersSignal;
    pthread_t waitClientsLoopThread;
    pthread_create(&waitClientsLoopThread, NULL, (void*)waitClientsLoop, &params);

    // Mando un paquete inicial a memoria para tener conexion. 
    t_package* initialPackageM = createPackage(KERNEL_MODULE);
    socketClientMemory = createConection(getLogger(), getKernelConfig()->IP_MEMORIA, getKernelConfig()->PUERTO_MEMORIA);
    sendPackage(initialPackageM, socketClientMemory);

    // Mando un paquete inicial a CPUDispatch para tener conexion. 
    t_package* initialPackageToCpuInterrupt = createPackage(KERNEL_MODULE_TO_CPU_INTERRUPT);
    socketClientCPUInterrupt = createConection(getLogger(), getKernelConfig()->IP_CPU, getKernelConfig()->PUERTO_CPU_INTERRUPT);
    sendPackage(initialPackageToCpuInterrupt, socketClientCPUInterrupt);

    // Mando un paquete inicial a CPUInterrupt para tener conexion. 
    t_package* initialPackageToCpuDispatch = createPackage(KERNEL_MODULE_TO_CPU_DISPATCH);
    socketClientCPUDispatch = createConection(getLogger(), getKernelConfig()->IP_CPU, getKernelConfig()->PUERTO_CPU_DISPATCH);
    sendPackage(initialPackageToCpuDispatch, socketClientCPUDispatch);

    destroyPackage(initialPackageM);
    destroyPackage(initialPackageToCpuInterrupt);
    destroyPackage(initialPackageToCpuDispatch);

    // Con el socket de CPUDispatch abro un server para escucharlo.
    initServerForASocket(socketClientCPUDispatch, serverKernelForCPU);
    initServerForASocket(socketClientMemory, serverKernelForMemory);

    logInitial();

    // Inicio consola de Kernel. Si termina la consola termina todo el programa.
    pthread_t kernelConsoleThread;
    pthread_create(&kernelConsoleThread, NULL, (void*)readKernelConsole, NULL);
    pthread_join(kernelConsoleThread, NULL);

///////////////////////////////////////////////////////////

/*

    t_package* initialPackageToMemory = createPackage(KERNEL_MODULE);
    t_package* initialPackageToCPUDispatch = createPackage(KERNEL_MODULE_TO_CPU_DISPATCH);
    t_package* initialPackageToCPUInterrupt = createPackage(KERNEL_MODULE_TO_CPU_INTERRUPT);
    int socketClientCPUDispatch = createConection(getLogger(), getKernelConfig()->IP_CPU, getKernelConfig()->PUERTO_CPU_DISPATCH);

    t_package* testPackageToMemory = createPackage(PACKAGE_FROM_KERNEL);
    char* msg1 = "Holaaaa, soy un mensaje de prueba desde el kernel.";
    addToPackage(testPackageToMemory, msg1, string_length(msg1) + 1); // (+1) para tener en cuenta el caracter nulo
    
    t_package* testPackageToCPUDispatch = createPackage(PACKAGE_FROM_KERNEL);
    char* msg2 = "Holaaaa, soy un mensaje de prueba desde el Kernel hacia Dispatch";
    addToPackage(testPackageToCPUDispatch, msg2, string_length(msg2) + 1); // (+1) para tener en cuenta el caracter nulo

    t_package* testPackageToCPUInterrupt = createPackage(PACKAGE_FROM_KERNEL);
    char* msg3 = "Holaaaa, soy un mensaje de prueba desde el Kernel hacia Interrupt";
    addToPackage(testPackageToCPUInterrupt, msg3, string_length(msg3) + 1); // (+1) para tener en cuenta el caracter nulo

    
    log_info(getLogger(), "Creando conexion con la CPU Dispatch. Se enviara un mensaje a la CPU Dispatch");
    


    int socketClientCPUDispatch = createConection(getLogger(), getKernelConfig()->IP_CPU, getKernelConfig()->PUERTO_CPU_DISPATCH);


    sendPackage(initialPackageToCPUDispatch, socketClientCPUDispatch);
    sendPackage(testPackageToCPUDispatch, socketClientCPUDispatch);
    releaseConnection(socketClientCPUDispatch);
    log_info(getLogger(), "Paquete enviado con exito.");
    

    log_info(getLogger(), "Creando conexion con la CPU Interrupt. Se enviara un mensaje a la CPU Interrupt");
    int socketClientCPUInterrupt = createConection(getLogger(), getKernelConfig()->IP_CPU, getKernelConfig()->PUERTO_CPU_INTERRUPT);
    sendPackage(initialPackageToCPUInterrupt, socketClientCPUInterrupt);
    sendPackage(testPackageToCPUInterrupt, socketClientCPUInterrupt);
    releaseConnection(socketClientCPUInterrupt);
    log_info(getLogger(), "Paquete enviado con exito.");



    log_info(getLogger(), "Creando conexion con la Memoria. Se enviara un mensaje a la Memoria");
    int socketClientMemory = createConection(getLogger(), getKernelConfig()->IP_MEMORIA, getKernelConfig()->PUERTO_MEMORIA);
    sendPackage(initialPackageToMemory, socketClientMemory);
    sendPackage(testPackageToMemory, socketClientMemory);
    releaseConnection(socketClientMemory);
    log_info(getLogger(), "Paquete enviado con exito.");


    destroyPackage(testPackageToMemory);
    destroyPackage(testPackageToCPUDispatch);
    destroyPackage(testPackageToCPUInterrupt);


    // Espero para ver si me llegan mensajes.
    // Esta linea es unicamente para testeo para el primer checkpoint, para saber que efectivamente funcionan las conexiones. Sera eliminada luego
    //sleep(60);

*/

    destroyResources();
    destroyInterfaces();

    // Lanzando la senial a los servidores de que no deben escuchar mas clientes ni realizar ninguna operacion.
    finishAllServersSignal();

    // Liberando todos los recursos.
    freeKernelConfig();
    

    sem_destroy(&semaphoreForIO);
    sem_destroy(&semNew);
    sem_destroy(&semReady);
    sem_destroy(&semExec);
    sem_destroy(&semBlock);
    sem_destroy(&semExit);
    sem_destroy(&semMultiProcessing);
    sem_destroy(&semMultiProgramming);
    sem_destroy(&semAddPid);

    destroyListMutex(pcbNewList);
    destroyListMutex(pcbReadyList);
    destroyListMutex(pcbExecList);
    destroyListMutex(pcbBlockList);
    destroyListMutex(pcbExitList);
    destroyListMutex(pcbReadyPriorityList);

    releaseConnection(socketClientMemory);
    releaseConnection(socketClientCPUDispatch);

    destroyLogger();

    return 0;
}
