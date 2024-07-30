#ifndef CLIENT_H_
#define CLIENT_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include "utils/utilsGeneral.h"

#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include "utils/debugMode.h"


// Estructura usada para contener info importante para un buffer.
typedef struct
{
	int size;
	void* stream;
} t_buffer;


// Estructura usada para contener info importante para un paquete.
typedef struct
{
	operationCode opCode;
	t_buffer* buffer;
} t_package;


// Se conecta al servidor que esta escuchando en la ip y el puerto especificado.
// Retorna el socket del cliente, o -1 si hubo un error, y loggea usando log_error.
int createConection(t_log* logger, char *ip, char* port);


// Envia el mensaje especificado al servidor.
// Esta funcion quedo del tp0, no deberia ser usada.
//void sendMessage(char* message, int socketClient);


// Crea un paquete.
t_package* createPackage(operationCode opCode);

// Crea un buffer. No deberia ser usado por fuera de su scope de utilidad.
void _createBuffer(t_package* package);


// Agrega un nuevo valor al paquete especificado. Este paquete ira creando una lista serializada.
// Cada llamada a esta funcion agrega un nuevo nodo al final de esa lista. Esta lista serializada sera enviada al servidor en forma de paquete.
// El servidor recibira esa lista serializada, la deserializara, y reconstruira la lista que estas confeccionando ahora mismo.
void addToPackage(t_package* package, void* value, int size);


// Envia el paquete especificado.
void sendPackage(t_package* package, int socketClient);


// Libera la conexion.
void releaseConnection(int socketClient);


// Libera la memoria de un paquete. Es necesario llamar a esta funcion cuando ya no se necesite usar un paquete.
void destroyPackage(t_package* package);


// Serializa un paquete. No deberia ser usado por fuera de su scope de utilidad.
void* _serializePackage(t_package* package, int bytes);


// Funcion que sirve para probar conexiones. Usada para el primer checkpoint.
// void readConsoleAndSendPackage(int socketClient);


/// @brief Envia al socket socketClient el codigo de operacion opCode. Esta funcion es util si se necesita enviar solamente un codigo de operacion y ningun paquete extra
/// @param socketClient El socket del cliente que va a enviar la informacion
/// @param opCode El codigo de operacion que se enviará
void sendOperation(int socketClient, operationCode opCode);



#endif 