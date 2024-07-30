#ifndef CPU_INSTRUCTION_CYCLE_H_
#define CPU_INSTRUCTION_CYCLE_H_


#include "fetch.h"
#include "decode.h"
#include "execute.h"
#include "checkInterrupt.h"
#include "essentials.h"


/// @brief Ejecuta un ciclo de instruccion completo (fetch, decode y execute)
void runInstructionCycle();







#endif