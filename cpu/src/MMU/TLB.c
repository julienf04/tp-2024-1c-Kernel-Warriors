#include "TLB.h"
#include "utilsCPU/config.h"
#include "connections/clientCPU.h"
#include "utilsCPU/logger.h"
#include <commons/collections/node.h>


TLBElement* TLBFIFO;


int fifoCounter;


t_list* TLBLRU;

int currentFrame;

sem_t semTLBMiss;


void setCurrentFrame(int value)
{
    currentFrame = value;
}

int getCurrentFrame()
{
    return currentFrame;
}


void initTLB()
{
    sem_init(&semTLBMiss, 0, 0);

    switch (getCPUConfig()->ALGORITMO_TLB)
    {
    case ALGORITMO_FIFO:
        initFIFOAlgorithm();
        break;
    
    case ALGORITMO_LRU:
        initLRUAlgorithm();
        break;
    }
}

void initFIFOAlgorithm()
{
    TLBFIFO = malloc(getCPUConfig()->CANTIDAD_ENTRADAS_TLB * sizeof(TLBElement));

    // Inicializo todas las entradas con pid = -1, lo que significa que no contienen informacion valida en este momento esas entradas.
    for (int i = 0; i < getCPUConfig()->CANTIDAD_ENTRADAS_TLB; i++)
    {
        TLBFIFO[i].pid = -1;
    }

    fifoCounter = 0;
}


void initLRUAlgorithm()
{
    TLBLRU = list_create();

    t_list_iterator* victimsIterator = list_iterator_create(TLBLRU);

    for (int i = 0; i < getCPUConfig()->CANTIDAD_ENTRADAS_TLB; i++)
    {
        TLBElement* newElement = malloc(sizeof(TLBElement));
        newElement->pid = -1; // Le asigno -1 para que no ocurra un TLB_HIT sin querer al principio

        list_iterator_add(victimsIterator, newElement);
    }

    list_iterator_destroy(victimsIterator);    
}


int getFrame(int pid, int page)
{
    int frame;


    /////////////////// CASO EN EL QUE LA TLB TIENE 0 ENTRADAS ///////////////////

    if (getCPUConfig()->CANTIDAD_ENTRADAS_TLB <= 0)
    {
        // Pido el frame a Memoria
        frame = waitFrameFromMemory(pid, page);

        // HACER EL LOG DEL TLB_MISS ACAAAAAA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        logTLBMiss(pid, page);
        
        return frame;
    }



    /////////////////// CASO EN EL QUE LA TLB TIENE 1 O MÁS ENTRADAS ///////////////////


    TLBResult result;
    

    //// Obtengo el frame de la TLB, o lo pido a Memoria y reemplazo una de las entradas segun el algoritmo
    switch (getCPUConfig()->ALGORITMO_TLB)
    {
    case ALGORITMO_FIFO:
        result = getFrameFIFO(pid, page, &frame);
        break;
    
    case ALGORITMO_LRU:
        result = getFrameLRU(pid, page, &frame);
        break;
    }
    
    // Log obligatorio de Obtener Marco
    logGetFrame(pid, page, frame);

    //// Hago los logs necesarios si ocurrió TLB_HIT o TLB_MISS
    switch (result)
    {
    case TLB_HIT:
        logTLBHit(pid, page);
        break;
    
    case TLB_MISS:
        logTLBMiss(pid, page);
        break;
    }


    return frame;
}

TLBResult getFrameFIFO(int pid, int page, int* frame)
{
    ////// La TLB tiene al menos 1 entrada

    TLBResult result = TLB_MISS;

    //// Recorro la TLB hasta encontrar el pid y pagina o hasta que llegue al final de la TLB

    for (int i = 0; i < getCPUConfig()->CANTIDAD_ENTRADAS_TLB && result == TLB_MISS; i++)
    {
        if (TLBFIFO[i].pid == pid && TLBFIFO[i].page == page)
        {
            result = TLB_HIT;
            *frame = TLBFIFO[i].frame;
        }
    }

    //// Si la pagina no se encuentra en la TLB, se la pide a la memoria, espera a que llegue el frame y reemplaza segun el algoritmo.

    if (result == TLB_MISS)
    {
        *frame = waitFrameFromMemory(pid, page);

        TLBFIFO[fifoCounter].pid = pid;
        TLBFIFO[fifoCounter].page = page;
        TLBFIFO[fifoCounter].frame = *frame;

        fifoCounter = (fifoCounter + 1) % getCPUConfig()->CANTIDAD_ENTRADAS_TLB;
    }

    return result;
}

TLBResult getFrameLRU(int pid, int page, int* frame)
{
    ///// La TLB tiene al menos 1 entrada

    TLBResult result = TLB_MISS;


    t_list_iterator* iterator = list_iterator_create(TLBLRU);

    //// Recorro la TLB hasta encontrar el pid y pagina o hasta que llegue al final de la TLB

    while (list_iterator_has_next(iterator) && result == TLB_MISS)
    {
        TLBElement* element = (TLBElement*)list_iterator_next(iterator);
        if (element->pid == pid && element->page == page)
        {
            result = TLB_HIT;
            *frame = element->frame;
            moveElementOfListToLast(iterator, element);
        }
    }

    //// Si la pagina no se encuentra en la TLB, se la pide a la memoria, espera a que llegue el frame y reemplaza segun el algoritmo.

    if (result == TLB_MISS)
    {
        *frame = waitFrameFromMemory(pid, page);

        TLBElement* element = (TLBElement*)list_remove(TLBLRU, 0);

        element->pid = pid;
        element->page = page;
        element->frame = *frame;

        list_add(TLBLRU, element);
    }
    
    list_iterator_destroy(iterator);

    return result;
}


void moveElementOfListToLast(t_list_iterator* iterator, void* actualElement)
{
    list_iterator_remove(iterator);

    // Voy al ultimo elemento de la lista
    while (list_iterator_has_next(iterator))
    {
        list_iterator_next(iterator);
    }
    
    list_iterator_add(iterator, actualElement);
}


int waitFrameFromMemory(int pid, int page)
{
    requestFrame(pid, page);

    sem_wait(&semTLBMiss);

    return getCurrentFrame();
}


void updateEntries(int pid, int amountOfPages)
{
    //// Dejo invalidas las entradas de las paginas que sean mayores a la cantidad de pagians y tenga igual pid.
    //// Esto segun algoritmo de reemplazo, ya que el trato es diferente porque se almacenan en estructuras distintas.
    switch (getCPUConfig()->ALGORITMO_TLB)
    {
    case ALGORITMO_FIFO:
        updateEntriesFIFO(pid, amountOfPages);
        break;
    
    case ALGORITMO_LRU:
        updateEntriesLRU(pid, amountOfPages);
        break;
    }
}

void updateEntriesFIFO(int pid, int amountOfPages)
{
    int lastPageNumber = amountOfPages - 1;

    for (int i = 0; i < getCPUConfig()->CANTIDAD_ENTRADAS_TLB; i++)
    {
        if (TLBFIFO[i].pid == pid && TLBFIFO[i].page > lastPageNumber)
        {
            TLBFIFO[i].pid = -1;
        }
    }
}

void updateEntriesLRU(int pid, int amountOfPages)
{
    int lastPageNumber = amountOfPages - 1;

    t_list_iterator* iterator = list_iterator_create(TLBLRU);

    while (list_iterator_has_next(iterator))
    {
        TLBElement* element = (TLBElement*)list_iterator_next(iterator);
        if (element->pid == pid && element->page > lastPageNumber)
        {
            element->pid = -1;
        }
    }

    list_iterator_destroy(iterator);
}