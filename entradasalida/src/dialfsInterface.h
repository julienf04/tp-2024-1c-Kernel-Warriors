#ifndef DIALFS_INTERFACES_H
#define DIALFS_INTERFACES_H

#include "interfaces.h"

void executeIOFSCreateAndSendResults();

bool executeIOFSCreate();

void executeIOFSDeleteAndSendResults();

void executeIOFSDelete();

void executeIOFSTruncateAndSendResults();

void executeIOFSTruncate();

void executeIOFSWriteAndSendResults();

void executeIOFSWrite();

void executeIOFSReadAndSendResults();

void executeIOFSRead();

/// @brief Pone como ocupado el primer bloque libre que encuentre. Retorna el indice de ese bloque.
/// @param blockIndex El indice del bloque reservado. Si el retorno de la funcion es false, no debería usarse este valor retornado.
/// @return Retorna true si se pudo encontrar un bloque libre. Retorna falso si todos los bloques ya estaban ocupados.
bool takeFirstFreeBlock(int* blockIndex);


/// @brief Compacta los archivos en los bloques del FS.
/// @param fileNameToSendToLast El nombre del archivo que debe ir al final.
void compactAndSendFileToLast(char* fileNameToSendToLast);






//////////////////////// FUNCIONES AUXILIARES ////////////////////////

/// @brief Retorna true si el archivo con nombre fileName es un archivo de usuario del FS (esto ocurre si es distinto a bloques.dat y bitmap.dat)
/// @param fileName El nombre del archivo
/// @return retorna true si es un archivo de usuario, sino falso.
bool isFileInFS(char* fileName);




#endif