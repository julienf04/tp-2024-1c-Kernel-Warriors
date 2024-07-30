#ifndef RESOURCES_H_
#define RESOURCES_H_

#include "kernel.h"
#include "generalPlanning.h"

// Va a instanciar los recursos del sistema segun el config
void initResources();

bool compareNameResource(void *data);

// Busca el recurso con el nombre dado y lo retorna. 
resource_t* foundResource(char* resourceName);


void addInstanceResource(resource_t* resource);

void subtractInstanceResource(resource_t* resource);

void destroyResources();

#endif
