#include "decode.h"
#include <commons/string.h>
#include <string.h>
#include "utilsCPU/logger.h"



instructionType decodeInstruction(char* instruction, void** outParams)
{
    logExecutingInstruction(getCurrentPID(), instruction);

    instructionType type;

    // Divido la instruccion por cada palabra escrita
    char** instructionSplitted = string_split(instruction, INSTRUCTION_SEPARATOR);

    // Me fijo qué tipo de instruccion es en base a la primera palabra de la instruccion
    if (strcmp(instructionSplitted[0], SET_STRING) == 0) type = SET_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], MOV_IN_STRING) == 0) type = MOV_IN_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], MOV_OUT_STRING) == 0) type = MOV_OUT_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], SUM_STRING) == 0) type = SUM_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], SUB_STRING) == 0) type = SUB_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], JNZ_STRING) == 0) type = JNZ_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], RESIZE_STRING) == 0) type = RESIZE_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], COPY_STRING_STRING) == 0) type = COPY_STRING_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], WAIT_STRING) == 0) type = WAIT_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], SIGNAL_STRING) == 0) type = SIGNAL_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], IO_GEN_SLEEP_STRING) == 0) type = IO_GEN_SLEEP_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], IO_STDIN_READ_STRING) == 0) type = IO_STDIN_READ_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], IO_STDOUT_WRITE_STRING) == 0) type = IO_STDOUT_WRITE_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], IO_FS_CREATE_STRING) == 0) type = IO_FS_CREATE_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], IO_FS_DELETE_STRING) == 0) type = IO_FS_DELETE_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], IO_FS_TRUNCATE_STRING) == 0) type = IO_FS_TRUNCATE_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], IO_FS_WRITE_STRING) == 0) type = IO_FS_WRITE_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], IO_FS_READ_STRING) == 0) type = IO_FS_READ_GET_INFO(instructionSplitted, outParams);
    else if (strcmp(instructionSplitted[0], EXIT_STRING) == 0) type = EXIT_GET_INFO(instructionSplitted, outParams);



    // Libero la memoria uasda para almacenar el string spliteado
    string_array_destroy(instructionSplitted);

    return type;
}



registerType _stringRegisterToType(char* regString)
{
    if (strcmp(regString, PC_STRING) == 0) return PC_TYPE;
    else if (strcmp(regString, AX_STRING) == 0) return AX_TYPE;
    else if (strcmp(regString, BX_STRING) == 0) return BX_TYPE;
    else if (strcmp(regString, CX_STRING) == 0) return CX_TYPE;
    else if (strcmp(regString, DX_STRING) == 0) return DX_TYPE;
    else if (strcmp(regString, EAX_STRING) == 0) return EAX_TYPE;
    else if (strcmp(regString, EBX_STRING) == 0) return EBX_TYPE;
    else if (strcmp(regString, ECX_STRING) == 0) return ECX_TYPE;
    else if (strcmp(regString, EDX_STRING) == 0) return EDX_TYPE;
    else if (strcmp(regString, SI_STRING) == 0) return SI_TYPE;
    else if (strcmp(regString, DI_STRING) == 0) return DI_TYPE;
}





instructionType SET_GET_INFO(char** instructionSplitted, void** outParams)
{
    SET_STRUCT* params = malloc(sizeof(SET_STRUCT));
    params->reg = _stringRegisterToType(instructionSplitted[1]);
    params->value = atoi(instructionSplitted[2]);

    *outParams = params;
    return SET_TYPE;
}

instructionType MOV_IN_GET_INFO(char** instructionSplitted, void** outParams)
{
    MOV_IN_STRUCT* params = malloc(sizeof(MOV_IN_STRUCT));
    params->data = _stringRegisterToType(instructionSplitted[1]);
    params->direction = _stringRegisterToType(instructionSplitted[2]);

    *outParams = params;
    return MOV_IN_TYPE;
}

instructionType MOV_OUT_GET_INFO(char** instructionSplitted, void** outParams)
{
    MOV_OUT_STRUCT* params = malloc(sizeof(MOV_OUT_STRUCT));
    params->direction = _stringRegisterToType(instructionSplitted[1]);
    params->data = _stringRegisterToType(instructionSplitted[2]);

    *outParams = params;
    return MOV_OUT_TYPE;
}

instructionType SUM_GET_INFO(char** instructionSplitted, void** outParams)
{
    SUM_STRUCT* params = malloc(sizeof(SUM_STRUCT));
    params->destination = _stringRegisterToType(instructionSplitted[1]);
    params->origin = _stringRegisterToType(instructionSplitted[2]);

    *outParams = params;
    return SUM_TYPE;
}

instructionType SUB_GET_INFO(char** instructionSplitted, void** outParams)
{
    SUB_STRUCT* params = malloc(sizeof(SUB_STRUCT));
    params->destination = _stringRegisterToType(instructionSplitted[1]);
    params->origin = _stringRegisterToType(instructionSplitted[2]);

    *outParams = params;
    return SUB_TYPE;
}

instructionType JNZ_GET_INFO(char** instructionSplitted, void** outParams)
{
    JNZ_STRUCT* params = malloc(sizeof(JNZ_STRUCT));
    params->reg = _stringRegisterToType(instructionSplitted[1]);
    params->instruction = atoi(instructionSplitted[2]);

    *outParams = params;
    return JNZ_TYPE;
}

instructionType RESIZE_GET_INFO(char** instructionSplitted, void** outParams)
{
    RESIZE_STRUCT* params = malloc(sizeof(RESIZE_STRUCT));
    params->size = atoi(instructionSplitted[1]);

    *outParams = params;
    return RESIZE_TYPE;
}

instructionType COPY_STRING_GET_INFO(char** instructionSplitted, void** outParams)
{
    COPY_STRING_STRUCT* params = malloc(sizeof(COPY_STRING_STRUCT));
    params->size = atoi(instructionSplitted[1]);

    *outParams = params;
    return COPY_STRING_TYPE;
}

instructionType WAIT_GET_INFO(char** instructionSplitted, void** outParams)
{
    WAIT_STRUCT* params = malloc(sizeof(WAIT_STRUCT));
    params->resource = string_duplicate(instructionSplitted[1]); // NO OLVIDARSE LIBERAR CON FREE EL STRING DUPLICADO CUANDO YA SEA USADO

    *outParams = params;
    return WAIT_TYPE;
}

instructionType SIGNAL_GET_INFO(char** instructionSplitted, void** outParams)
{
    SIGNAL_STRUCT* params = malloc(sizeof(SIGNAL_STRUCT));
    params->resource = string_duplicate(instructionSplitted[1]); // NO OLVIDARSE LIBERAR CON FREE EL STRING DUPLICADO CUANDO YA SEA USADO

    *outParams = params;
    return SIGNAL_TYPE;
}

instructionType IO_GEN_SLEEP_GET_INFO(char** instructionSplitted, void** outParams)
{
    IO_GEN_SLEEP_STRUCT* params = malloc(sizeof(IO_GEN_SLEEP_STRUCT));
    params->interface = string_duplicate(instructionSplitted[1]); // NO OLVIDARSE LIBERAR CON FREE EL STRING DUPLICADO CUANDO YA SEA USADO
    params->workUnits = atoi(instructionSplitted[2]);

    *outParams = params;
    return IO_GEN_SLEEP_TYPE;
}

instructionType IO_STDIN_READ_GET_INFO(char** instructionSplitted, void** outParams)
{
    IO_STDIN_READ_STRUCT* params = malloc(sizeof(IO_STDIN_READ_STRUCT));
    params->interface = string_duplicate(instructionSplitted[1]); // NO OLVIDARSE LIBERAR CON FREE EL STRING DUPLICADO CUANDO YA SEA USADO
    params->direction = _stringRegisterToType(instructionSplitted[2]);
    params->size = _stringRegisterToType(instructionSplitted[3]);

    *outParams = params;
    return IO_STDIN_READ_TYPE;
}

instructionType IO_STDOUT_WRITE_GET_INFO(char** instructionSplitted, void** outParams)
{
    IO_STDOUT_WRITE_STRUCT* params = malloc(sizeof(IO_STDOUT_WRITE_STRUCT));
    params->interface = string_duplicate(instructionSplitted[1]); // NO OLVIDARSE LIBERAR CON FREE EL STRING DUPLICADO CUANDO YA SEA USADO
    params->direction = _stringRegisterToType(instructionSplitted[2]);
    params->size = _stringRegisterToType(instructionSplitted[3]);

    *outParams = params;
    return IO_STDOUT_WRITE_TYPE;
}

instructionType IO_FS_CREATE_GET_INFO(char** instructionSplitted, void** outParams)
{
    IO_FS_CREATE_STRUCT* params = malloc(sizeof(IO_FS_CREATE_STRUCT));
    params->interface = string_duplicate(instructionSplitted[1]); // NO OLVIDARSE LIBERAR CON FREE EL STRING DUPLICADO CUANDO YA SEA USADO
    params->fileName = string_duplicate(instructionSplitted[2]); // NO OLVIDARSE LIBERAR CON FREE EL STRING DUPLICADO CUANDO YA SEA USADO

    *outParams = params;
    return IO_FS_CREATE_TYPE;
}

instructionType IO_FS_DELETE_GET_INFO(char** instructionSplitted, void** outParams)
{
    IO_FS_DELETE_STRUCT* params = malloc(sizeof(IO_FS_DELETE_STRUCT));
    params->interface = string_duplicate(instructionSplitted[1]); // NO OLVIDARSE LIBERAR CON FREE EL STRING DUPLICADO CUANDO YA SEA USADO
    params->fileName = string_duplicate(instructionSplitted[2]); // NO OLVIDARSE LIBERAR CON FREE EL STRING DUPLICADO CUANDO YA SEA USADO

    *outParams = params;
    return IO_FS_DELETE_TYPE;
}

instructionType IO_FS_TRUNCATE_GET_INFO(char** instructionSplitted, void** outParams)
{
    IO_FS_TRUNCATE_STRUCT* params = malloc(sizeof(IO_FS_TRUNCATE_STRUCT));
    params->interface = string_duplicate(instructionSplitted[1]); // NO OLVIDARSE LIBERAR CON FREE EL STRING DUPLICADO CUANDO YA SEA USADO
    params->fileName = string_duplicate(instructionSplitted[2]); // NO OLVIDARSE LIBERAR CON FREE EL STRING DUPLICADO CUANDO YA SEA USADO
    params->size = _stringRegisterToType(instructionSplitted[3]);

    *outParams = params;
    return IO_FS_TRUNCATE_TYPE;
}

instructionType IO_FS_WRITE_GET_INFO(char** instructionSplitted, void** outParams)
{
    IO_FS_WRITE_STRUCT* params = malloc(sizeof(IO_FS_WRITE_STRUCT));
    params->interface = string_duplicate(instructionSplitted[1]); // NO OLVIDARSE LIBERAR CON FREE EL STRING DUPLICADO CUANDO YA SEA USADO
    params->fileName = string_duplicate(instructionSplitted[2]); // NO OLVIDARSE LIBERAR CON FREE EL STRING DUPLICADO CUANDO YA SEA USADO
    params->direction = _stringRegisterToType(instructionSplitted[3]);
    params->size = _stringRegisterToType(instructionSplitted[4]);
    params->pointer = _stringRegisterToType(instructionSplitted[5]);

    *outParams = params;
    return IO_FS_WRITE_TYPE;
}

instructionType IO_FS_READ_GET_INFO(char** instructionSplitted, void** outParams)
{
    IO_FS_READ_STRUCT* params = malloc(sizeof(IO_FS_READ_STRUCT));
    params->interface = string_duplicate(instructionSplitted[1]); // NO OLVIDARSE LIBERAR CON FREE EL STRING DUPLICADO CUANDO YA SEA USADO
    params->fileName = string_duplicate(instructionSplitted[2]); // NO OLVIDARSE LIBERAR CON FREE EL STRING DUPLICADO CUANDO YA SEA USADO
    params->direction = _stringRegisterToType(instructionSplitted[3]);
    params->size = _stringRegisterToType(instructionSplitted[4]);
    params->pointer = _stringRegisterToType(instructionSplitted[5]);

    *outParams = params;
    return IO_FS_READ_TYPE;
}

instructionType EXIT_GET_INFO(char** instructionSplitted, void** outParams)
{
    return EXIT_TYPE;
}
