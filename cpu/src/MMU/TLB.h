#ifndef CPU_TLB_H_
#define CPU_TLB_H_

#include <commons/collections/list.h>
#include <semaphore.h>


// Struct que contiene la info necesaria de cada entrada (elemento) de la TLB.
typedef struct
{
    int pid;
    int page;
    int frame;
} TLBElement;


typedef enum
{
    TLB_MISS,
    TLB_HIT
} TLBResult;


// Array de elementos de TLB. Es la TLB en sí misma.
extern TLBElement* TLBFIFO;


// Numero usado solo para el algoritmo FIFO. Dice cuál es el proximo indice a eliminar de la TLB.
// Debe incrementarse en 1 cada vez que es reemplazada una entrada, y volver a 0 cuando alcanzó su valor maximo (la cantidad de entradas de la TLB)
extern int fifoCounter;


// Lista usada solo para el algoritmo LRU. Contiene los indices de la TLB que deben ser reemplazados, en orden.
// Por ejemplo, si la lista es [3,1,2] , el indice 3 de la TLB debe ser reemplazado primero, luego el 1 y luego el 2.
// Las nuevas paginas que se incorporen deben añadirse al final de la lista.
extern t_list* TLBLRU;

// El frame del pid y pagina actual.
extern int currentFrame;


// Semaforo usado para cuando ocurre un TLB_MISS. La TLB debe pedirle el frame a la Memoria.
// Este semaforo sirve para avisar que ya llegó el frame de la Memoria.
extern sem_t semTLBMiss;


// Settea al frame actual el valor indicado.
void setCurrentFrame(int value);

// Obtiene el frame actual.
int getCurrentFrame();



// Inicializa la TLB.
void initTLB();


// Inicializa el algoritmo FIFO de la TLB (en caso de que ese sea el algoritmo a usar)
void initFIFOAlgorithm();


// Inicializa el algoritmo LRU de la TLB (en caso de que ese sea el algoritmo a usar)
void initLRUAlgorithm();


/// @brief Obtiene el frame correspondiente a una determinada pagina de un determinado proceso.
/// Si la TLB lo contiene, lo retorna directamente, sino se lo pide a la Memoria y lo guarda en la TLB
/// @param pid Process id del proceso.
/// @param page Pagina para buscar el frame
/// @return Retorna el frame correspondiente al pid y la pagina
int getFrame(int pid, int page);


/// @brief Obtiene el frame. Si es necesario, reemplaza con el algoritmo FIFO
/// @param pid El process id del proceso
/// @param page // La pagina a buscarle el frame
/// @param frame // Retorna el frame. No debe pasarse NULL o un puntero invalido
/// @return Retorna el resultado de la obtencion del frame. Si la TLB contenia la pagina, retorna TLB_HIT.
/// Si tuvo que pedirselo a la Memoria, retorna TLB_MISS
TLBResult getFrameFIFO(int pid, int page, int* frame);



/// @brief Obtiene el frame. Si es necesario, reemplaza con el algoritmo LRU
/// @param pid El process id del proceso
/// @param page // La pagina a buscarle el frame
/// @param frame // Retorna el frame. No debe pasarse NULL o un puntero invalido
/// @return Retorna el resultado de la obtencion del frame. Si la TLB contenia la pagina, retorna TLB_HIT.
/// Si tuvo que pedirselo a la Memoria, retorna TLB_MISS
TLBResult getFrameLRU(int pid, int page, int* frame);



/// @brief Mueve el elemento actual al final de la lista
/// @param iterator El iterador de la lista, el cual debe estar posicionado en el elemento que se debe mover al final de la lista
/// @param actualElement La data del elemento actual
void moveElementOfListToLast(t_list_iterator* iterator, void* actualElement);



/// @brief Le pide el frame a la Memoria y espera a que le llegue
/// @param pid Process id
/// @param page La pagina a pedir
/// @return Retorna el frame
int waitFrameFromMemory(int pid, int page);



/// @brief Deja invalidas las entradas del pid cuyas paginas sean mayores que la cantidad de paginas.
/// Este caso en el que las paginas sean mayores que la cantidad paginas puede ocurrir si se hace un RESIZE X, y luego un RESIZE Y, con X > Y
/// @param pid El Process ID
/// @param amountOfPages La cantidad de paginas que tiene el proceso.
void updateEntries(int pid, int amountOfPages);


/// @brief Esta la implementacion de updateEntries en caso de que el algoritmo de reemplazo que se esté usado sea FIFO.
/// @param pid El Process ID
/// @param amountOfPages La cantidad de paginas que tiene el proceso.
void updateEntriesFIFO(int pid, int amountOfPages);


/// @brief Esta la implementacion de updateEntries en caso de que el algoritmo de reemplazo que se esté usado sea LRU.
/// @param pid El Process ID
/// @param amountOfPages La cantidad de paginas que tiene el proceso.
void updateEntriesLRU(int pid, int amountOfPages);

#endif