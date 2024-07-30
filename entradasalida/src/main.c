#include "interfaces.h"
#include "utils/config.h"

int main(int argc, char* argv[])
{
    // Inicio el logger general del modulo. Siempre deberia ser la primera sentencia a ejecutar del main.
    initLogger("IO.log", "entrada salida", true, LOG_LEVEL_INFO);

    // Obtengo la configuracion general.
    char* path = askForConfigPath();
    initIOConfig(path);
    free(path);

    // Inicializacion de semaforos y reserva de memoria
    sem_init(&semaphoreReceiveDataFromMemory, 0, 0);
    sem_init(&semaphoreSendDataToMemory, 0, 0);
    //sem_init(&semaphoreForIOFSWrite, 0, 0);
    //sem_init(&semaphoreForIOFSRead, 0, 0);
    sem_init(&semaphoreForModule, 0, 0);

    // Se crea el nombre de la interfaz
    if (argc > 1) createInterface(argv[1]);
    else
    {   
        // Esta variante pide el nombre cuando no se lo ingresa como parámetro del main, es para pruebas, luego se eliminará
        char *name = readline("Nombre de la interfaz: ");
        createInterface(name);
        free(name);
    }

    t_package* initialPackageForKernel = createPackage(IO_MODULE);
    //log_info(getLogger(), "Creando conexion con el Kernel. Se enviara un mensaje al Kernel.");
    socketKernel = createConection(getLogger(), getIOConfig()->IP_KERNEL, getIOConfig()->PUERTO_KERNEL);
    sendPackage(initialPackageForKernel, socketKernel);
    //log_info(getLogger(), "Paquete enviado con exito al Kernel.");

    t_package* initialPackageForMemory = createPackage(IO_MODULE);
    //log_info(getLogger(), "Creando conexion con la memoria. Se enviara un mensaje a la Memoria.");
    socketMemory = createConection(getLogger(), getIOConfig()->IP_MEMORIA, getIOConfig()->PUERTO_MEMORIA);
    sendPackage(initialPackageForMemory, socketMemory);
    //log_info(getLogger(), "Paquete enviado con exito a la memoria.");

    initServerForASocket(socketKernel, serverIOForKernel);
    initServerForASocket(socketMemory, serverIOForMemory);

    //log_info(getLogger(), "Se enviara el tipo y nombre de la interfaz al Kernel.");
    sendInterfaceToKernel();
    //log_info(getLogger(), "Tipo y nombre de la interfaz enviado al Kernel.");
    
    log_info(getLogger(), "-----------------------------------------------");
    log_info(getLogger(), "MODULO IO INICIADO ;) <3");
    log_info(getLogger(), "-----------------------------------------------");

    // Se espera en el main a que los hilos de servidor terminen (que cualquiera de los dos termine)
    sem_wait(&semaphoreForModule);

    destroyPackage(initialPackageForKernel);
    destroyPackage(initialPackageForMemory);

    // Liberando todos los recursos
    freeIOConfig();
    destroyLogger();
    closeBlocksFile();
    closeBitmapFile();
    closeAllFiles();

    // Señal para todos los servidores para que dejen de escuchar clientes
    finishAllServersSignal();

    // Se cierran los sockets de conexión
    releaseConnection(socketKernel);
    releaseConnection(socketMemory);

    // Destrucción de semáforos
    sem_destroy(&semaphoreSendDataToMemory);
    sem_destroy(&semaphoreReceiveDataFromMemory);
    sem_destroy(&semaphoreForModule);

    return 0;
}