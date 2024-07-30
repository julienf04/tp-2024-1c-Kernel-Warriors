#include "utils.h"
#include <semaphore.h>


int initServer(t_log* logger, char* port)
{
	#ifdef DEBUG_UTILS
	log_info(logger, "Iniciando el servidor");
	#endif

	struct addrinfo hints,*servinfo;


	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, port, &hints, &servinfo);

	// creamos el socket
	int socketServer = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol); // Retorna -1 si hubo un error.
	if (socketServer == -1) 
	{
        log_error(logger, "Error al crear el socket del servidor");
        freeaddrinfo(servinfo);
        return -1;
    }

	// Asociamos el socket a un puerto
	int bindSuccess = bind(socketServer, servinfo->ai_addr, servinfo->ai_addrlen); // Retorna 0 si "bindeo" con exito, retorna -1 si hubo un error.
	if (bindSuccess == -1) 
	{
        log_error(logger, "Error al enlazar el socket del servidor");
        close(socketServer);
        freeaddrinfo(servinfo);
        return -1;
    }

	// Escuchamos las conexiones entrantes
	//int listenSuccess = listen(socketServer, SOMAXCONN); Retorna 0 si "escucha" con exito, retorna -1 si hubo un error. Por ahora sin usar.
	if(listen(socketServer, SOMAXCONN) == -1)
	{
		log_error(logger, "Error al escuchar las conexiones entrantes");
        close(socketServer);
        freeaddrinfo(servinfo);
        return -1;
	}

	freeaddrinfo(servinfo);

	#ifdef DEBUG_UTILS
	log_info(logger, "Servidor iniciado con exito");
	#endif

	return socketServer;
}

int waitClient(t_log* logger, int socketServer)
{
	#ifdef DEBUG_UTILS
	log_info(logger, "Esperando a cliente");
	#endif

	// Aceptamos un nuevo cliente
    int socketClient = accept(socketServer, NULL, NULL); // Retorna -1 si hubo un error.
    if (socketClient == -1)
	{
        log_error(logger, "Error al aceptar la conexion del cliente");
        return -1;
    }

	#ifdef DEBUG_UTILS
    log_info(logger, "Se conecto un cliente!");
	#endif

    return socketClient;
}

operationCode getOperation(int socketClient)
{
	operationCode opCode;
	if(recv(socketClient, &opCode, sizeof(int), MSG_WAITALL) > 0)
		return opCode;
	else
	{
		close(socketClient);
		return ERROR;
	}

}

void* _getBuffer(int* size, int socketClient)
{
	void * buffer;

	recv(socketClient, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socketClient, buffer, *size, MSG_WAITALL);

	return buffer;
}

void getMessage(t_log* logger, int socketClient)
{
	int size;
	char* buffer = _getBuffer(&size, socketClient);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* getPackage(int socketClient)
{
	int totalSize;
	int offset = 0;
	void * buffer;
	t_list* values = list_create();
	int eachSize;

	buffer = _getBuffer(&totalSize, socketClient);
	while(offset < totalSize)
	{
		memcpy(&eachSize, buffer + offset, sizeof(int));
		offset+=sizeof(int);
		char* value = malloc(eachSize);
		memcpy(value, buffer+offset, eachSize);
		offset+=eachSize;
		list_add(values, value);
	}
	free(buffer);
	return values;
}



void waitClientsLoop(waitClientsLoopParams* params)
{
    // Inicio el servidor
    int socketServer = initServer(params->logger, params->portToListen);

    if (socketServer == -1)
    {
        log_error(params->logger, "Error: no se pudo iniciar el servidor.\n");
        exit(EXIT_FAILURE);
    }


    while (!(*(params->finishLoopSignal)))
    {
        (params->eachIterationFunc)(socketServer);
    }
}



void initServerForASocket(int socketClient, void(*serverFunction)(int*))
{
	pthread_t thread;
    int *socketClientParam = (int *)malloc(sizeof(int));
    *socketClientParam = socketClient;
    pthread_create(&thread, NULL, (void*)serverFunction, socketClientParam);
    pthread_detach(thread);
}