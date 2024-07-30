#include "clientIO.h"

void sendInterfaceToKernel()
{   
    t_package* package = createPackage(IO_SEND_INTERFACE);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1);
    addToPackage(package, &interfaceData.type, sizeof(interfaceType));

    sendPackage(package, socketKernel);

    destroyPackage(package);
}

void sendIOGenSleepResultsToKernel()
{   
    t_package* package = createPackage(IO_OK);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1);

    sendPackage(package, socketKernel);
    
    destroyPackage(package);
}

void sendIOStdinReadResultsToKernel()
{   
    t_package* package = createPackage(IO_OK);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1); // +1 por el caracter nulo \0

    sendPackage(package, socketKernel);
    
    destroyPackage(package);
}

void sendIOStdoutWriteResultsToKernel()
{   
    t_package* package = createPackage(IO_OK);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1); // +1 por el caracter nulo \0

    sendPackage(package, socketKernel);
    
    destroyPackage(package);
}

void sendResultsFromStdinToMemory(void* data, uint32_t physicalDirection, uint32_t size)
{   
    t_package* package = createPackage(WRITE_MEMORY);

    t_paramsForStdinInterface *params = (t_paramsForStdinInterface*)interfaceData.currentOperation.params;

    addToPackage(package, &interfaceData.currentOperation.pid, sizeof(int));
    addToPackage(package, data, size);
    addToPackage(package, &physicalDirection, sizeof(uint32_t));
    addToPackage(package, &size, sizeof(uint32_t));

    sendPackage(package, socketMemory);
    
    destroyPackage(package);
}

/*
void sendResultsFromIOFSReadToMemory()
{
    log_info(getLogger(), "Se enviara la lectura de la operacion IO_FS_READ a la memoria.");
    t_package* package = createPackage(WRITE_MEMORY);

    t_paramsForIOFSWriteOrRead *params = (t_paramsForIOFSWriteOrRead*)interfaceData.currentOperation.params;

    addToPackage(package, resultsForIOFSRead.resultsFromRead, params->registerSize);
    addToPackage(package, &params->registerDirection, sizeof(uint32_t));
    addToPackage(package, &params->registerSize, sizeof(uint32_t));

    sendPackage(package, socketKernel);

    log_info(getLogger(), "Lectura de la operacion IO_FS_READ enviada a la memoria.");
    
    destroyPackage(package);
}
*/

void sendIOReadRequestToMemory(uint32_t physicalDirection, uint32_t size)
{   
    if (interfaceData.currentOperation.operation == IO_STDOUT_WRITE)
    {
        t_package* package = createPackage(READ_MEMORY);

        t_paramsForStdoutInterface *params = (t_paramsForStdoutInterface*)interfaceData.currentOperation.params;

        addToPackage(package, &interfaceData.currentOperation.pid, sizeof(uint32_t));
        addToPackage(package, &physicalDirection, sizeof(uint32_t));
        addToPackage(package, &size, sizeof(uint32_t));

        sendPackage(package, socketMemory);
        
        destroyPackage(package);
    }
    else if (interfaceData.currentOperation.operation == IO_FS_WRITE)
    {
        t_package* package = createPackage(READ_MEMORY);

        t_paramsForIOFSWriteOrRead *params = (t_paramsForIOFSWriteOrRead*)interfaceData.currentOperation.params;

        addToPackage(package, &interfaceData.currentOperation.pid, sizeof(uint32_t));
        addToPackage(package, &physicalDirection, sizeof(uint32_t));
        addToPackage(package, &size, sizeof(uint32_t));

        sendPackage(package, socketMemory);
        
        destroyPackage(package);
    }
}

void sendIOFSCreateResultsToKernel(int success)
{
    t_package* package;
    
    //if (success) package = createPackage(IO_OK);
    //else package = createPackage(IO_NOT_OK);

    package = createPackage(IO_OK);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1);

    sendPackage(package, socketKernel);
    
    destroyPackage(package);
}

void sendIOFSDeleteResultsToKernel()
{
    t_package* package = createPackage(IO_OK);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1);

    sendPackage(package, socketKernel);
    
    destroyPackage(package);
}

void sendIOFSTruncateResultsToKernel()
{
    t_package* package = createPackage(IO_OK);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1);

    sendPackage(package, socketKernel);
    
    destroyPackage(package);
}

void sendIOFSWriteResultsToKernel()
{
    t_package* package = createPackage(IO_OK);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1);

    sendPackage(package, socketKernel);
    
    destroyPackage(package);
}

void sendIOFSReadResultsToKernel()
{
    t_package* package = createPackage(IO_OK);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1);

    sendPackage(package, socketKernel);
    
    destroyPackage(package);
}