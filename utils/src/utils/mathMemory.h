#ifndef MATH_MEMORY_H_
#define MATH_MEMORY_H_


#include "math.h"
#include "debugMode.h"




/// @brief Retorna la cantidad de bytes que representan la fragmentacion interna de la ultima pagina de la memoria de usuario.
/// @param bytesAllocated La cantidad de bytes reservados por un proceso.
/// @return Retorna la fragmentacion interna.
int getInternalFragmentation(int pageSize, int bytesAllocated);



/// @brief Retorna la cantidad de bytes que estan reservados en la ultima pagina.
/// @param pageSize El tamaño de pagina
/// @param internalFragmentation La fragmentacion interna que ocurre en la ultima pagina.
/// @return Retorna la cantidad de byte reservados en la ultima pagina.
int getAmountOfBytesInTheLastPage(int pageSize, int internalFragmentation);


/// @brief Retorna la cantidad de bytes que están reservados en base a la cantidad de paginas y la fragmentacion interna
/// @param pageSize El tamaño de pagina
/// @param amountOfPages Cantidad de paginas reservadas
/// @param internalFragmentation Cantidad de bytes que representan la fragmentacion interna que quedó (o quedaría) en la ultima pagina
/// /// (es decir, la cantidad de bytes que faltan para completar la ultima pagina).
/// @return Retorna la cantidad de bytes reservados.
int getAmountOfBytesAllocated(int pageSize, int amountOfPages, int internalFragmentation);



/// @brief Retorna la cantidad de paginas que se necesitan (o que fueron ya reservadas) para reservar la cantidad de bytes especificados.
/// @param pageSize El tamaño de pagina
/// @param bytesAllocated La cantidad de bytes.
/// @return Retorna la cantidad de paginas que se necesitan.
int getAmountOfPagesAllocated(int pageSize, int bytesAllocated);



/// @brief Retorna la cantidad de paginas que ya no se necesitan para liberar la cantidad de bytes especificados.
/// Se considera que los bytes reservados por la ultima pagina son 0 bytes, y como minimo se libera una pagina.
/// @param pageSize El tamaño de pagina
/// @param bytesToFree La cantidad de bytes.
/// @return Retorna la cantidad de paginas que ya no se necesitan.
int getAmountOfPagesToFree(int pageSize, int bytesToFree);


/// @brief Calcula la direccion fisica a partir del tamaño de página, el frame y el offset.
/// @param pageSize El tamaño de pagina
/// @param frame El frame
/// @param offset El offset
/// @return Retorna frame * pageSize + offset
int calculatePhysicalAddr(int pageSize, int frame, int offset);





#endif