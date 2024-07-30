#ifndef CPU_FETCH_H_
#define CPU_FETCH_H_


#include "essentials.h"
#include "utils/utilsGeneral.h"


extern instructionString* currentInstructionString;

/// @brief Pertenece a la fase fetch del ciclo de la CPU, Le pide a la memoria la siguiente instruccion, y espera a que llegue. La instruccion será almacenada en la variable global instructionString.
/// @param PC El Program Counter necesario para decirle a la memoria cuál instruccion debe enviarme
/// @return Retorna la estructura que contiene lo que envió la memoria sobre la instruccion que debe ejecutar la CPU (es decir, la instruccion en forma de string).
/// @warning Es necesario que quien use esta funcion libere la memoria de la estructura que retorna la funcion, asi como tambien la de todos sus campos que usen memoria dinamica
instructionString* getNextInstruction(int PID, uint32_t PC);



/// @brief Setter para la variable que contiene informacion de la instruccion llegada desde la memoria
/// @param value Nuevo valor de la variable
void setCurrentInstructionString(instructionString* value);


instructionString* getCurrentInstructionString();



#endif