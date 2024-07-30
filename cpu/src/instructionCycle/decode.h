#ifndef CPU_DECODE_H_
#define CPU_DECODE_H_

#include <stdio.h>
#include <stdlib.h>
#include "essentials.h"







/// @brief Decodifica la instruccion instruction en forma de string, para saber qué tipo de instruccion es y cuáles son sus parametros
/// @param instruction La instruccion en forma de string
/// @param outParams Debe ser un puntero a NULL o a basura (es decir, no se debe haber reservado memoria al pasar por parametro este puntero). Se crea la estructura correspondiente usando malloc y se almacenan sus parametros segun lo que viene en la instruccion instruction. Se retorna esa estructura (ES NECESARIO LIBERAR MEMORIA)
/// @warning ES NECESARIO LIBERAR LA MEMORIA DEL outParams CON FREE SI SE USA ESTA FUNCIÓN, ASI COMO TAMBIEN LAS VARIABLES INTERNAS DE LA ESTRUCTURA QUE HAYAN RESERVADO MEMORIA CON MALLOC (como los strings (char*) por ejemplo)
/// @return Retorna el tipo de instruccion
instructionType decodeInstruction(char* instruction, void** outParams);


/// @brief Convierte un registro en forma de string (como por ejemplo "AX") en su tipo de registro correspondiente
/// @param regString String que representa el registro
/// @return Retorna el tipo del registro
registerType _stringRegisterToType(char* regString);





//////////////////// FUNCIONES AUXILIARES PARA OBTENER LA INFORMACION NECESARIA DE UNA INSTRUCCION A PARTIR DE SU STRING ////////////////////

instructionType SET_GET_INFO(char** instructionSplitted, void** outParams);

instructionType MOV_IN_GET_INFO(char** instructionSplitted, void** outParams);

instructionType MOV_OUT_GET_INFO(char** instructionSplitted, void** outParams);

instructionType SUM_GET_INFO(char** instructionSplitted, void** outParams);

instructionType SUB_GET_INFO(char** instructionSplitted, void** outParams);

instructionType JNZ_GET_INFO(char** instructionSplitted, void** outParams);

instructionType RESIZE_GET_INFO(char** instructionSplitted, void** outParams);

instructionType COPY_STRING_GET_INFO(char** instructionSplitted, void** outParams);

instructionType WAIT_GET_INFO(char** instructionSplitted, void** outParams);

instructionType SIGNAL_GET_INFO(char** instructionSplitted, void** outParams);

instructionType IO_GEN_SLEEP_GET_INFO(char** instructionSplitted, void** outParams);

instructionType IO_STDIN_READ_GET_INFO(char** instructionSplitted, void** outParams);

instructionType IO_STDOUT_WRITE_GET_INFO(char** instructionSplitted, void** outParams);

instructionType IO_FS_CREATE_GET_INFO(char** instructionSplitted, void** outParams);

instructionType IO_FS_DELETE_GET_INFO(char** instructionSplitted, void** outParams);

instructionType IO_FS_TRUNCATE_GET_INFO(char** instructionSplitted, void** outParams);

instructionType IO_FS_WRITE_GET_INFO(char** instructionSplitted, void** outParams);

instructionType IO_FS_READ_GET_INFO(char** instructionSplitted, void** outParams);

instructionType EXIT_GET_INFO(char** instructionSplitted, void** outParams);




#endif