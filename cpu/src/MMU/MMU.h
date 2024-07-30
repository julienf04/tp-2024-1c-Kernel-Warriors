#ifndef CPU_MMU_H_
#define CPU_MMU_H_


#include "utils/utilsGeneral.h"


// El tamaño de las paginas en bytes de la memoria
extern int TAM_PAGINA;


// Struct que contiene la info de la pagina y el offset correspondiente a una direccion logica
typedef struct
{
    int page;
    int offset;
} pageAndOffset;


// Settea el valor value a la variable que contiene el tamaño de pagina en bytes de la memoria
void setTamPagina(int value);

// Obtiene el tamaño de pagina  en bytes de la memoria
int getTamPagina();


// Retorna la esctructura con los valores setteados de los parametros
physicalAddressInfo createPhysicalAddressInfo(int physicalAddress, int size);

// Retorna la esctructura con los valores setteados de los parametros. outPhysicalAddressInfo debe tener un valor valido, no debe ser NULL ni ser un puntero a basura
void createPhysicalAddressInfoParam(int physicalAddress, int size, physicalAddressInfo* outPhysicalAddressInfo);



/// @brief Obtiene el numero de pagina y el desplazamiento correspondiente a la direccion logica pasada por parametro
/// @param logicalAddress La direccion logica
/// @param outPageAndOffset Retorna un struct que contiene el numero de pagina y el desplazamiento. No debe ser NULL.
void getPageAndOffset(int logicalAddress, pageAndOffset* outPageAndOffset);




/// @brief Obtiene todas las direcciones fisicas junto con al tamaño a leer o escribir en cada una de ellas.
/// @param pid Process ID
/// @param logicalAddress // La direccion logica
/// @param size El tamaño total a leer o escribir
/// @param outPhysicalAddressesInfo // Retorna un array, donde cada elemento tiene la informacion en qué direccion fisica leer o escribir y cuantos bytes en cada una.
/// Hace un malloc.
/// @return Retorna el tamaño del array retornado (la cantidad de elementos)
int getAllPhysicalAddresses(int pid, int logicalAddress, int size, physicalAddressInfo** outPhysicalAddressesInfo);





#endif