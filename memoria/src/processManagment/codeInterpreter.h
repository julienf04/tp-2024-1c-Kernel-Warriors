#ifndef MEMORY_PSEUDOCODE_INTERPRETER_H_
#define MEMORY_PSEUDOCODE_INTERPRETER_H_


#include <stdio.h>
#include "essentials.h"
#include "utils/listMutex.h"


/// @brief Retorna la instruccion correspondiente al PID y el PC pasados como parametro.
/// @param PID El Process ID
/// @param PC El Program Counter
/// @return Retorna la instruccion en forma de string.
char* getInstruction(int PID, int PC);
















#endif