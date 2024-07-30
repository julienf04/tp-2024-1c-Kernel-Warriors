#include "inOut.h"

interface_t *createInterface(char* nameInterface, interfaceType typeInterface)
{
    interface_t *newInterface = malloc(sizeof(interface_t));

    newInterface->name = malloc(string_length(nameInterface) + 1); // +1 por el nulo
    strcpy(newInterface->name, nameInterface);

    newInterface->processAssign = NULL;
    newInterface->interfaceType = typeInterface;
    newInterface->flagKillProcess = false;
    newInterface->isBusy = false;
    newInterface->blockList = initListMutex();

    return newInterface;
}

char* interfaceNameToFind;

bool compareNameInterface(void *data)
{
    interface_t *interface = (interface_t *)data;
    return string_equals_ignore_case(interface->name, interfaceNameToFind);
}

interface_t *foundInterface(char* nameRequestInterface)
{
    interfaceNameToFind = nameRequestInterface;

    interface_t *interfaceFound = (interface_t*)list_find_mutex(interfacesList, compareNameInterface);

    if (interfaceFound == NULL) return NULL;

    return interfaceFound;
}

pcb_t* auxPid;

bool comparePidAssignInInterface(void *data)
{
    interface_t *interface = (interface_t *)data;
    return interface->processAssign == auxPid;
}

interface_t *foundInterfaceByProcessPidAssign(pcb_t* pcb)
{
    auxPid = pcb;

    interface_t *interfaceFound = (interface_t*)list_find_mutex(interfacesList, comparePidAssignInInterface);

    if (interfaceFound == NULL) return NULL;

    return interfaceFound;
}

int auxSocketVal;

bool compareSocketValAssignOfInterface(void *data)
{
    interface_t *interface = (interface_t *)data;
    return *(interface->socket) == auxSocketVal;
}

interface_t *foundInterfaceBySocket(int socket)
{
    auxSocketVal = socket;

    interface_t *interfaceFound = (interface_t*)list_find_mutex(interfacesList, compareSocketValAssignOfInterface);

    if (interfaceFound == NULL) return NULL;

    return interfaceFound;
}

void destroyInterfaces()
{
    int interfacesSize = list_mutex_size(interfacesList);

    for(int i = 0; i < interfacesSize; i++){
        interface_t *interfaceToDestroy = list_pop(interfacesList);

        free(interfaceToDestroy->name);
        destroyListMutex(interfaceToDestroy->blockList);
        free(interfaceToDestroy);
    }

    destroyListMutex(interfacesList);
}