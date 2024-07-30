#include "config.h"
#include <stdio.h>
#include <readline/readline.h>
#include <unistd.h>




// Convierte un doble puntero a char leido de un archivo de configuracion a una lista de strings. Retorna esa lista
t_list* getListOfStringsFromConfig(t_config* config, char* key, t_log* logger, const char* errorMessage)
{
    char** listChars = config_get_array_value(config, key);

    if (listChars == NULL)
    {
        log_error(logger, "%s",errorMessage);
        return NULL;
    }
    
    return _fromConfigToListOfStrings(listChars);
}


// Funcion auxiliar para agregar todos los strings a una lista
t_list* _fromConfigToListOfStrings(char** listChars)
{
    t_list* list = list_create();

    int i = 0;
    while (listChars[i] != NULL)
    {
        list_add(list, strdup(listChars[i])); 
        i++;
    }

    i = 0;
    while (listChars[i] != NULL)
    {
        free(listChars[i]);
        i++;
    }
    free(listChars);

    return list;
}


// Convierte un doble puntero a char leido de un archivo de configuracion a una lista de ints. Retorna esa lista
t_list* getListOfIntsFromConfig(t_config* config, char* key, t_log* logger, const char* errorMessage)
{
    char** listChars = config_get_array_value(config, key);

    if (listChars == NULL)
    {
        log_error(logger, "%s" ,errorMessage);
        return NULL;
    }

    return _fromConfigToListOfInts(listChars);
}

// Funcion auxiliar para agregar todos los int a una lista
t_list* _fromConfigToListOfInts(char** listChars)
{
    t_list* list = list_create();

    int i = 0;
    while (listChars[i] != NULL)
    {
        int *temp = malloc(sizeof(int)); // Sin este malloc, no persiste el valor asignado por fuera de esta funcion la variable int *temp.
        *temp = atoi(listChars[i]);
        //int temp = atoi(listChars[i]); //Para evitar el warning, agrego un temporal
        list_add(list, temp);
        i++;
    }

    i = 0;
    while (listChars[i] != NULL)
    {
        free(listChars[i]);
        i++;
    }
    free(listChars);

    return list;
} 


char* askForConfigPath()
{
    char* path;

    path = readline("¡Hola! Introduce el path del archivo de configuracion: ");

    // Mientras que el archivo no exista, va a seguir preguntando por el archivo de configuracion
    while (!fileExists(path))
    {
        free(path);
        path = readline("¡Oh no, el archivo no existe!, ¿lo escribiste correctamente? Introducelo de nuevo: ");
    }
    
    return path;
}

bool fileExists(char* path)
{
    return access(path, F_OK) == 0;
}