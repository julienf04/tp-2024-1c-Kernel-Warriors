#include "resources.h"

void initResources()
{
    t_list* resourcesList = getKernelConfig()->RECURSOS;
    t_list* resourcesInstancesList = getKernelConfig()->INSTANCIAS_RECURSOS;

    int sizeResourcesName = list_size(resourcesList);
    int sizeResourcesInstances = list_size(resourcesInstancesList);

    if(sizeResourcesInstances != sizeResourcesName) {
        log_error(getLogger(), "Error: Instacias de recursos y recursos distintas");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < sizeResourcesName; i++){
        resource_t *resource = malloc(sizeof(resource_t));
        resource->name = (char*)list_get(resourcesList, i);
        resource->instances = *(int*)list_get(resourcesInstancesList, i);
        resource->blockList = initListMutex();
        pthread_mutex_init(&(resource->mutexForInstances), NULL);
        list_push(resourcesBlockList, resource);        
    }
}

char* resourceNameToFind;

bool compareNameResource(void *data)
{
    resource_t *resource = (resource_t *)data;
    return string_equals_ignore_case(resource->name, resourceNameToFind);
}

resource_t* foundResource(char* resourceName)
{
    resourceNameToFind = resourceName;

    resource_t *resourceFound = (resource_t*)list_find(resourcesBlockList->list, compareNameResource);

    if(resourceFound == NULL){
        return NULL;
    }

    return resourceFound;
}

void addInstanceResource(resource_t* resource)
{
    pthread_mutex_lock(&(resource->mutexForInstances));
    resource->instances++;
    pthread_mutex_unlock(&(resource->mutexForInstances));
}

void subtractInstanceResource(resource_t* resource)
{
    pthread_mutex_lock(&(resource->mutexForInstances));
    resource->instances--;
    pthread_mutex_unlock(&(resource->mutexForInstances));
}

void destroyResources()
{
    int sizeResources = list_mutex_size(resourcesBlockList); //Necesito el tamaño de uno porque si llega a este punto no necesita verificar que sean iguales.

    for(int i = 0; i < sizeResources; i++){
        resource_t *resourceToDestroy = list_pop(resourcesBlockList);
        destroyListMutex(resourceToDestroy->blockList);
        pthread_mutex_destroy(&(resourceToDestroy->mutexForInstances));
        free(resourceToDestroy);
    }

    destroyListMutex(resourcesBlockList);
}