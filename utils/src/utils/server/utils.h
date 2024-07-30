#ifndef SERVER_H_
#define SERVER_H_

#include <stdlib.h>
#include <stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>
#include<netdb.h>
#include "utils/utilsGeneral.h"
#include "utils/debugMode.h"


// Structu para llenar con los valores correspondientes y enviarlo como parametro a la funcion waitClientsLoop
typedef struct
{
    t_log* logger;
    char* portToListen;
    void (*eachIterationFunc)(int);
    bool* finishLoopSignal;
} waitClientsLoopParams;


// Funcion auxiliar que recibe el buffer del socket del cliente especificado.
// No deberia ser uasda por fuera su scope de utilidad.
void* _getBuffer(int* size, int socketClient);

// Inicia el servidor. Crea la conexion para que el socket sea capaz de escuchar en el puerto determinado.
// Use la funcion waitClient para esperar a que un cliente se conecte al puerto. Use la funcion getOperation o getPackage para quedarse esperando la llegada de un paquete.
// Loggea lo que va ocurriendo.
// Retorna el socket de servidor, o -1 si hubo algun error.
int initServer(t_log* logger, char* port);


// Espera a que un cliente se conecte con el socket del servidor especificado.
int waitClient(t_log* logger, int socketServer);


// Cuando el socket del cliente especificado envia un paquete, el servidor lo recibe.
// Esta funcion se queda esperando a recibir un paquete por parte de un cliente
// Retorna un lista con todos los mensajes que envio el socket del cliente
t_list* getPackage(int socketClient);


// Cuando el socket del cliente especificado envia un paquete, el servidor lo recibe.
// Esta funcion recibe el paquete e imprime por pantalla su contenido.
// Esta funcion quedo del tp0, no deberia ser usada.
void getMessage(t_log* logger, int socketClient);


// Espera a que le llegue un paquete desde el socket del cliente especificado. Retorna el codigo de operacion de ese paquete
operationCode getOperation(int socketClient);


// Inicializa el servidor y el semaforo pasado como parametro. 
// Espera en un loop a los clientes y les crea su hilo para recibir paquetes correspondiente (si es que no se llego al maximo de clientes)
void waitClientsLoop(waitClientsLoopParams* params);



/// @brief Inicia un servidor en un nuevo thread que escuchará solamente al socket especificado
/// @param socketClient El socket del cliente al que escuchará el servidor
/// @param serverFunction La funcion que se ejecutará en el thread (es decir, el servidor). El servidor debe retornar void y recibir un puntero a int, que será el socket del cliente al que escuchará
void initServerForASocket(int socketClient, void(*serverFunction)(int*));



void initMainServer();



#endif