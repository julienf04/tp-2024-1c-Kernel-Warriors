#ifndef UTILS_CONFIG_H_
#define UTILS_CONFIG_H_

#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>
#include <stdlib.h>
#include "debugMode.h"

t_list* getListOfStringsFromConfig(t_config* config, char* key, t_log* logger, const char* errorMessage);
t_list* _fromConfigToListOfStrings(char** listChars);

t_list* getListOfIntsFromConfig(t_config* config, char* key, t_log* logger, const char* errorMessage);
t_list* _fromConfigToListOfInts(char** listChars);


/// @brief Le pregunta al usuario cuál es el path del archivo de configuracion, y retorna ese path.
/// En caso de que el path no sea valido porque el archivo no existe o por cualquier otro error, le seguirá preguntando nuevamente.
/// @return Retorna el path valido introducido por el usuario.
/// @warning Se debe liberar la memoria usando del path retornado, sino quedará un memory leak.
char* askForConfigPath();


/// @brief 
/// @param path El path.
/// @return Retorna true si existe el archivo del path especificado, sino falso.
bool fileExists(char* path);


#endif
