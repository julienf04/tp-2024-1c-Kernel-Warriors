#include "utils.h"


void* _serializePackage(t_package* package, int bytes)
{
	void * magic = malloc(bytes);
	int offset = 0;

	memcpy(magic + offset, &(package->opCode), sizeof(int));
	offset+= sizeof(int);
	memcpy(magic + offset, &(package->buffer->size), sizeof(int));
	offset+= sizeof(int);
	memcpy(magic + offset, package->buffer->stream, package->buffer->size);
	offset+= package->buffer->size;

	return magic;
}

int createConection(t_log* logger, char *ip, char* port)
{
	struct addrinfo hints;
	struct addrinfo *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, port, &hints, &servinfo);


	// Ahora vamos a crear el socket.
	int socketClient = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol); // Retorna -1 si hubo un error.
	if (socketClient == -1)
	{
		log_error(logger, "Hubo un error al crear el socket");
		return -1;
	}

	// Ahora que tenemos el socket, vamos a conectarlo

	int connectSuccess = connect(socketClient, servinfo->ai_addr, servinfo->ai_addrlen); // Retorna 0 si se conecto con exito, retorna -1 si hubo un error.
	if (connectSuccess == -1)
	{
		log_error(logger, "Hubo un error al conectarse con el servidor");
		return -1;
	}


	freeaddrinfo(servinfo);

	return socketClient;
}


/*
void sendMessage(char* message, int socketClient)
{
	t_package* package = malloc(sizeof(t_package));

	package->opCode = MESSAGE;
	package->buffer = malloc(sizeof(t_buffer));
	package->buffer->size = strlen(message) + 1;
	package->buffer->stream = malloc(package->buffer->size);
	memcpy(package->buffer->stream, message, package->buffer->size);

	int bytes = package->buffer->size + 2*sizeof(int);

	void* toSend = _serializePackage(package, bytes);

	send(socketClient, toSend, bytes, 0);

	free(toSend);
	destroyPackage(package);
}
*/

void _createBuffer(t_package* package)
{
	package->buffer = malloc(sizeof(t_buffer));
	package->buffer->size = 0;
	package->buffer->stream = NULL;
}

t_package* createPackage(operationCode opCode)
{
	t_package* package = malloc(sizeof(t_package));
	package->opCode = opCode;
	_createBuffer(package);
	return package;
}

void addToPackage(t_package* package, void* value, int size)
{
	package->buffer->stream = realloc(package->buffer->stream, package->buffer->size + size + sizeof(int));

	memcpy(package->buffer->stream + package->buffer->size, &size, sizeof(int));
	memcpy(package->buffer->stream + package->buffer->size + sizeof(int), value, size);

	package->buffer->size += size + sizeof(int);
}

void sendPackage(t_package* package, int socketClient)
{
	int bytes = package->buffer->size + 2*sizeof(int);
	void* toSend = _serializePackage(package, bytes);

	send(socketClient, toSend, bytes, 0);

	free(toSend);
}

void destroyPackage(t_package* package)
{
	free(package->buffer->stream);
	free(package->buffer);
	free(package);
}

void releaseConnection(int socketClient)
{
	close(socketClient);
}


void sendOperation(int socketClient, operationCode opCode)
{
	if(send(socketClient, &opCode, sizeof(operationCode), 0) < 0)
	{
		close(socketClient);
		exit(EXIT_FAILURE);
		//return ERROR;
	}
}



/*
void readConsoleAndSendPackage(int socketClient)
{
 	// Ahora toca lo divertido!
	char* read;
	t_package* package = createPackage();

	// Leemos y esta vez agregamos las lineas al paquete
	read = readline("> ");

	while (!is_empty_string(read))
	{
		addToPackage(package, read, strlen(read) + 1); // (+ 1) para tener en cuenta el caracter nulo '\0'
		free(read);
		read = readline("> ");
	}
	
	sendPackage(package, socketClient);

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	free(read);
	destroyPackage(package);
}

*/