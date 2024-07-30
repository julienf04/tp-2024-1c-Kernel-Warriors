#include "debugUtils.h"





#ifdef DEBUG_MEMORY

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <commons/string.h>
#include "logger.h"
#include <commons/memory.h>
#include "paging/memoryUser.h"


void printMemory()
{
    //mem_hexdump(getMemoryUser(), getMemoryConfig()->TAM_MEMORIA);
}


void printMemoryChars()
{

}


void printMemoryThread(void* ignore)
{
    char* command = readline("");
    char** commandSplitted = string_split(command, " ");
    while (true)
    {

        if (string_equals_ignore_case(command, "HELP")
            || string_equals_ignore_case(command, "H")
            || string_equals_ignore_case(command, "AYUDA"))
        {
            printf("Comandos:\n"
                    "[MEM / MEMORY / MEMORIA] : Muestra el valor actual de toda la memoria.\n"
                    "[MEM / MEMORY / MEMORIA] [CHAR / CHARS / CHARACTER / CHARACTERS / CARACTER / CARACTERES / C] : Muestra el valor actual de toda la memoria mostrando los caracteres en ASCII de cada posicion de memoria.\n"
                    "Q / QUIT : Sale  del bucle");
        }
        else if (string_equals_ignore_case(commandSplitted[0], "MEMORY")
            || string_equals_ignore_case(commandSplitted[0], "MEMORIA")
            || string_equals_ignore_case(commandSplitted[0], "MEM"))
        {
            if (string_equals_ignore_case(commandSplitted[1], "CHAR")
                || string_equals_ignore_case(commandSplitted[1], "CHARS")
                || string_equals_ignore_case(commandSplitted[1], "CHARACTER")
                || string_equals_ignore_case(commandSplitted[1], "CHARACTERS")
                || string_equals_ignore_case(commandSplitted[1], "CARACTER")
                || string_equals_ignore_case(commandSplitted[1], "CARACTERES")
                || string_equals_ignore_case(commandSplitted[1], "C"))
            {
                printMemoryChars();
            }
            else
            {
                printMemory();
            }
        }

        free(command);
        string_array_destroy(commandSplitted);
        char* command = readline("");
    }

    free(command);
    string_array_destroy(commandSplitted);
}

#endif
