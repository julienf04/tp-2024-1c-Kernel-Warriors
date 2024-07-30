#include "debugUtils.h"





#ifdef DEBUG_CPU

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <commons/string.h>
#include "logger.h"


void printRegisters()
{
    log_info(getLogger(), "PC = %d, AX = %d, BX = %d, CX = %d, DX = %d, EAX = %d, EBX = %d, ECX = %d, EDX = %d, SI = %d, DI = %d",
                        getPC(), getAX(), getBX(), getCX(), getDX(), getEAX(), getEBX(), getECX(), getEDX(), getSI(), getDI());
}


void printRegistersThread(void* ignore)
{
    char* command = readline("");
    while (true)
    {

        if (string_equals_ignore_case(command, "HELP")
            || string_equals_ignore_case(command, "H")
            || string_equals_ignore_case(command, "AYUDA"))
        {
            printf("Comandos:\n"
                    "REGS / REGISTERS / REGISTROS : Muestra el valor actual de todos los registros.\n"
                    "Q / QUIT : Sale  del bucle");
        }
        else if (string_equals_ignore_case(command, "REGISTERS")
            || string_equals_ignore_case(command, "REGISTROS")
            || string_equals_ignore_case(command, "REGS"))
        {
            printRegisters();
        }

        free(command);
        char* command = readline("");
    }

    free(command);
    
}

#endif
