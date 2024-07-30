#include "console.h"



//Habilita una consola y valida las instrucciones
void readKernelConsole(){

    flagExecutingScript = false;

    log_info(getLogger(), "\nConsola iniciada\n");
    char* read;
    read = readline("");

    while(1){
        if(string_equals_ignore_case(read, "SALIR")){
            log_info(getLogger(), "Apagando el sistema.");
            break;
        }
        if(!_isAnInstruction(read)){
            log_error(getLogger(), "Comando de consola no reconocido.");
            free(read);
            read = readline("");
            continue;
        }

        attendInstruction(read);
        free(read);
        read = readline("");
    } //Para terminar la consola se pone algo vacio
    free(read);
}


//Valida si hay alguna instruccion con lo ingresado
bool _isAnInstruction(char* instruction){
    char** consoleCommand = string_split(instruction, " ");

    if(string_equals_ignore_case(consoleCommand[0], "INICIAR_PROCESO")){
        if(string_array_size(consoleCommand) == 2) {
            string_array_destroy(consoleCommand);
            return true;
            } //Se fija si tiene la cantidad de parametros que pide la instruccion
    }
    else if(string_equals_ignore_case(consoleCommand[0], "EJECUTAR_SCRIPT")){
        if(string_array_size(consoleCommand) == 2) {
            string_array_destroy(consoleCommand);
            return true;
            } //Se fija si tiene la cantidad de parametros que pide la instruccion
    }
    else if(string_equals_ignore_case(consoleCommand[0], "FINALIZAR_PROCESO")){
        if(string_array_size(consoleCommand) == 2) {
            string_array_destroy(consoleCommand);
            return true;
            } //Se fija si tiene la cantidad de parametros que pide la instruccion
    }
    else if(string_equals_ignore_case(consoleCommand[0], "INICIAR_PLANIFICACION")){
        string_array_destroy(consoleCommand);
        return true;
    }
    else if(string_equals_ignore_case(consoleCommand[0], "DETENER_PLANIFICACION")){
        string_array_destroy(consoleCommand);
        return true;
    }
    else if(string_equals_ignore_case(consoleCommand[0], "MULTIPROGRAMACION")){
        if(string_array_size(consoleCommand) == 2) {
            string_array_destroy(consoleCommand);
            return true;
            } //Se fija si tiene la cantidad de parametros que pide la instruccion
    }
    else if(string_equals_ignore_case(consoleCommand[0], "PROCESO_ESTADO")){
        string_array_destroy(consoleCommand);
        return true;
    }

    string_array_destroy(consoleCommand);
    return false;
}

bool flagAuxStopPlanning = false;

//Atiende la instruccion
void attendInstruction(char* instruction)
{
    char** consoleCommand = string_split(instruction, " ");

    if(string_equals_ignore_case(consoleCommand[0], "INICIAR_PROCESO")){
        pthread_t initProcessThread;
        char* paramInitProcessThread = malloc(string_length(consoleCommand[1]) + 1);
        strcpy(paramInitProcessThread, consoleCommand[1]);
        pthread_create(&initProcessThread, NULL, (void*)addPcbToNew, paramInitProcessThread);
        pthread_detach(initProcessThread);
    }
    else if(string_equals_ignore_case(consoleCommand[0], "FINALIZAR_PROCESO")){
        pthread_t killProcessThread;
        int *paramkillProcessThread = malloc(sizeof(int));
        *paramkillProcessThread = atoi(consoleCommand[1]);
        pthread_create(&killProcessThread, NULL, (void*)killProcess, paramkillProcessThread);
        pthread_detach(killProcessThread);
        //killProcess(atoi(consoleCommand[1]));
    }
    else if(string_equals_ignore_case(consoleCommand[0], "DETENER_PLANIFICACION")){
        if (!flagAuxStopPlanning)sem_wait(&semPausePlanning);
        flagAuxStopPlanning = true;
    }
    else if(string_equals_ignore_case(consoleCommand[0], "INICIAR_PLANIFICACION")){
        if(flagAuxStopPlanning) sem_post(&semPausePlanning);
        flagAuxStopPlanning = false;
    }
    else if(string_equals_ignore_case(consoleCommand[0], "EJECUTAR_SCRIPT")){
        pthread_t executeScriptThread;
        char* paramExecuteScriptThread = malloc(string_length(consoleCommand[1]) + 1);
        strcpy(paramExecuteScriptThread, consoleCommand[1]);
        pthread_create(&executeScriptThread, NULL, (void*)executeScript, paramExecuteScriptThread);
        pthread_detach(executeScriptThread);
    }
    else if(string_equals_ignore_case(consoleCommand[0], "MULTIPROGRAMACION")){
        pthread_t changeMultiprogrammingThread;
        int *paramChangeMultiprogrammingThread = malloc(sizeof(int));
        *paramChangeMultiprogrammingThread = atoi(consoleCommand[1]);
        pthread_create(&changeMultiprogrammingThread, NULL, (void*)changeMultiprogramming, paramChangeMultiprogrammingThread);
        pthread_detach(changeMultiprogrammingThread);
    }
    else if(string_equals_ignore_case(consoleCommand[0], "PROCESO_ESTADO")){
        showProcessByState();
    }

    string_array_destroy(consoleCommand);
}

void executeScript(char* path)
{
    FILE *fileScript = fopen(path, "r");

    if (fileScript == NULL) {

        log_info(getLogger(), "No se encontro el archivo de script.");
        free(path);
        return;
    }

    flagExecutingScript = true;

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    while (1) {

        pthread_mutex_lock(&mutexOrderProcessByScript);

        read = getline(&line, &len, fileScript);

        if (read == -1) break;

        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }

        if (_isAnInstruction(line)) {
            attendInstruction(line);
        } else {
            log_info(getLogger(), "Comando de consola no reconocido");
        }

    }

    pthread_mutex_unlock(&mutexOrderProcessByScript);
    flagExecutingScript = false;

    pthread_mutex_destroy(&mutex);
    free(line);
    fclose(fileScript);
    free(path);
}

void showProcessByState()
{
    char* listProcessNew =  _listProcess(pcbNewList->list);
    char* listProcessReady =  _listProcess(pcbReadyList->list);
    char* listProcessExec =  _listProcess(pcbExecList->list);
    char* listProcessBlock =  _listProcess(pcbBlockList->list);
    char* listProcessExit = _listProcess(pcbExitList->list);

    log_info(getLogger(), listProcessNew, "PCB_NEW");
    log_info(getLogger(), listProcessReady, "PCB_READY");

    if(algorithm == VRR){
        char* listProcessReadyPlus = _listProcess(pcbReadyPriorityList->list);
        log_info(getLogger(), listProcessReadyPlus, "PCB_READY_PLUS");
        free(listProcessReadyPlus);
    }

    log_info(getLogger(), listProcessExec, "PCB_EXEC");
    log_info(getLogger(), listProcessBlock, "PCB_BLOCK");
    log_info(getLogger(), listProcessExit, "PCB_EXIT");

    free(listProcessNew);
    free(listProcessReady);
    free(listProcessExec);
    free(listProcessBlock);
    free(listProcessExit);
}

char* _listProcess(t_list *list)
{
    char* pids = string_new();
    string_append(&pids, "Estado %s");// Ready %s: [");
    string_append(&pids, " : [");

    for (int i = 0; i < list_size(list); i++)
    {
        pcb_t *process = list_get(list, i);
        char *pid = string_from_format("%d", process->pid);
        string_append(&pids, " ");
        string_append(&pids, pid);

        free(pid);
    }
    string_append(&pids, " ]");

    return pids;
}

void logInitial()
{
    log_info(getLogger(), "-----------------------------------------------");
    log_info(getLogger(), "MODULO KERNEL INICIADO <3");
    log_info(getLogger(), "-----------------------------------------------");
}

void changeMultiprogramming(int* paramChangeMultiprogrammingThread)
{
    int newMultiprogramming = *paramChangeMultiprogrammingThread;
    int prevMultiprogramming = getKernelConfig()->GRADO_MULTIPROGRAMACION;
    int difAux;
    
    if(newMultiprogramming <= 0){

        log_error(getLogger(), "El grado de multiprogramacion debe ser mayor que 0 y tiene que ser un numero!");

    } else {
        if(list_mutex_is_empty(pcbBlockList) && list_mutex_is_empty(pcbExecList) && list_mutex_is_empty(pcbReadyList)){

            if(newMultiprogramming > prevMultiprogramming){

                difAux = newMultiprogramming - prevMultiprogramming;

                while (difAux > 0){
                    sem_post(&semMultiProgramming);
                    difAux--;
                }

            } else if (prevMultiprogramming > newMultiprogramming){

                difAux = prevMultiprogramming - newMultiprogramming;
                
                while (difAux > 0){
                    sem_wait(&semMultiProgramming);
                    difAux--;
                }
            }

        } else {

            if(newMultiprogramming > prevMultiprogramming){

                difAux = newMultiprogramming - prevMultiprogramming;

                while(difAux > 0){
                    sem_post(&semMultiProgramming);
                    difAux--; 
                }

            } else if (prevMultiprogramming > newMultiprogramming) {
                //Tener cuidado despues de salir de este caso.

                difAux = prevMultiprogramming - newMultiprogramming;

                if(diffBetweenNewAndPrevMultiprogramming > 0){

                    diffBetweenNewAndPrevMultiprogramming =+ difAux;

                } else {

                    diffBetweenNewAndPrevMultiprogramming = difAux;

                }

            }
        }

        getKernelConfig()->GRADO_MULTIPROGRAMACION = newMultiprogramming;
        log_info(getLogger(), "Se cambio el grado de multiprogramacion a: %d", newMultiprogramming);

    }

    free(paramChangeMultiprogrammingThread);
}