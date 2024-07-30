#include "MMU.h"

#include "TLB.h"
#include "utils/mathMemory.h"
#include <math.h>

int TAM_PAGINA;


void setTamPagina(int value)
{
    TAM_PAGINA = value;
}

int getTamPagina()
{
    return TAM_PAGINA;
}



physicalAddressInfo createPhysicalAddressInfo(int physicalAddress, int size)
{
    physicalAddressInfo info;
    info.physicalAddress = physicalAddress;
    info.size = size;
    return info;
}


void createPhysicalAddressInfoParam(int physicalAddress, int size, physicalAddressInfo* outPhysicalAddressInfo)
{
    outPhysicalAddressInfo->physicalAddress = physicalAddress;
    outPhysicalAddressInfo->size = size;
}


void getPageAndOffset(int logicalAddress, pageAndOffset* outPageAndOffset)
{
    outPageAndOffset->page = floor(logicalAddress / getTamPagina());
    outPageAndOffset->offset = logicalAddress - outPageAndOffset->page * getTamPagina();
}


int getAllPhysicalAddresses(int pid, int logicalAddress, int size, physicalAddressInfo** outPhysicalAddressesInfo)
{
    ////////// CASOS DE CORTE //////////
    // Si no hay que escribir o leer ninguna cantidad de bytes en esa direccion, no hago nada y retorno 0.
    if (size <= 0)
    {   
        return 0;
    }



    ////////// CASOS COMUNES, AL MENOS UNA DIRECCION FISICA ES NECESARIA //////////

    int amountOfPhysAddr;
    int amountOfPhysAddrWithoutFirstAndLast;
    int sizeAfterFirstPhysAddr;
    int sizeFirstPhysAddr;
    int sizeLastPhysAddr;


    // Obtengo la pagina y el offset dentro de esa pagina
    pageAndOffset logicalAddressSplitted;
    getPageAndOffset(logicalAddress, &logicalAddressSplitted);


    // Hay que tener en cuenta que en la primera y ultima pagina se podria necesitar un tamaño para lectura o escritura "especiales"
    // (es decir, hay que calcular cuanto es ese tamaño). Para resolver eso, la primera y ultima pagina las pongo manualmente, mientras que en
    // las paginas del medi siempre va a escribir o leer un tamaño igual al tamaño de pagina.

    // Hago todos los calculos necesarios para determinar informacion del tamaño de la primera y ultima pagina, y me fijo cuantas paginas hay "en el medio"

    // Si solo hay una direccion fisica, es decir, una sola pagina involucrada,
    // el tamaño de esa unica direccion fisica es el mismo que se pasó por parametro a la funcion,
    // sino el tamaño es lo que le falta al offset para llegar al ultimo de la pagina.
    bool isThereOnePage = logicalAddressSplitted.offset + size <= getTamPagina();
    sizeFirstPhysAddr = isThereOnePage ? size : getTamPagina() - logicalAddressSplitted.offset;

    // El tamaño que falta por leer o escribir en las siguientes paginas. Si hay que leer o escribir una sola pagina, el resultado de la operacion es siempre 0.
    sizeAfterFirstPhysAddr = size - sizeFirstPhysAddr;

    // Obtengo el tamaño a leer o escribir de la ultima pagina. Si hay que leer o escribir una sola pagina, el resultado de la operacion es siempre 0.
    int internalFragmentation = getInternalFragmentation(getTamPagina(), sizeAfterFirstPhysAddr);
    sizeLastPhysAddr = isThereOnePage ? 0 : getTamPagina() - internalFragmentation;

    // La cantidad de paginas que hay que leer o escribir sin contar la prinera y ultima pagina, ya que estos tienen un trato especial
    // porque pueden leer o escribir tamaños diferentes, mientras que las paginas del medio siempre van a leer o escribir el tamaño de pagina
    amountOfPhysAddrWithoutFirstAndLast = getAmountOfPagesAllocated(getTamPagina(), sizeAfterFirstPhysAddr) - 1; // -1 porque no tengo que contar la ultima pagina

    // La cantidad de paginas totales que hay que leer o escribir, contando la primera y ultima pagina.
    amountOfPhysAddr = amountOfPhysAddrWithoutFirstAndLast + 2; // + 2 porque tengo que contar la primer y ultima pagina.


    // Reservo memoria para todas las direcciones fisicas que necesito.
    *outPhysicalAddressesInfo = malloc(amountOfPhysAddr * sizeof(physicalAddressInfo));

    int i = 0; // Contador para ir asignando en el array

    // Esta variable contendrá el valor del frame.
    int frame;
    // Esta variable contendrá la direccion fiscia.
    int physicalAddress;

    // Pongo la primera physicalAddressInfo
    frame = getFrame(pid, logicalAddressSplitted.page + i);
    physicalAddress = calculatePhysicalAddr(getTamPagina(), frame, logicalAddressSplitted.offset);

    createPhysicalAddressInfoParam(physicalAddress, sizeFirstPhysAddr, *outPhysicalAddressesInfo + i);

    i++;


    // Pongo las physicalAddressInfo "del medio"
    while (i < amountOfPhysAddrWithoutFirstAndLast + 1) // + 1 para tener en cuenta que la primera pagina ya fue puesta
    {
        frame = getFrame(pid, logicalAddressSplitted.page + i);
        physicalAddress = calculatePhysicalAddr(getTamPagina(), frame, 0);

        createPhysicalAddressInfoParam(physicalAddress, getTamPagina(), *outPhysicalAddressesInfo + i);

        i++;
    }

    
    // Pongo la ultima physicalAddressInfo si todavia sobran bytes para leer o escribir en memoria
    if (sizeLastPhysAddr > 0)
    {
        frame = getFrame(pid, logicalAddressSplitted.page + i);
        physicalAddress = calculatePhysicalAddr(getTamPagina(), frame, 0);

        createPhysicalAddressInfoParam(physicalAddress, sizeLastPhysAddr, *outPhysicalAddressesInfo + i);
    }


    return amountOfPhysAddr;
}
