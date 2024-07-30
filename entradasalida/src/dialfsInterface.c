#include "dialfsInterface.h"

void executeIOFSCreateAndSendResults()
{
    log_info(getLogger(), "PID: %d - Operacion: IO_FS_CREATE", (int)interfaceData.currentOperation.pid);

    delayFS();

    t_paramsForIOFSCreateOrDelete *params = (t_paramsForIOFSCreateOrDelete*)interfaceData.currentOperation.params;

    log_info(getLogger(), "PID: %d - Crear Archivo: %s", (int)interfaceData.currentOperation.pid, params->fileName);


    int success = executeIOFSCreate();

    sendIOFSCreateResultsToKernel(success);

    interfaceData.currentOperation.operation = IO_NULL;
    free(interfaceData.currentOperation.params);
    interfaceData.currentOperation.pid = -1;
}

bool executeIOFSCreate()
{
    int firstFreeBlockIndex;
    bool success;
    
    success = takeFirstFreeBlock(&firstFreeBlockIndex);

    if (success)
    {
        t_paramsForIOFSCreateOrDelete *params = (t_paramsForIOFSCreateOrDelete*)interfaceData.currentOperation.params;
        
        char *fullFileName = getFullFileName(params->fileName);


        // Creo el archivo si no existe.
        FILE *file = fopen(fullFileName, "a+");
        fclose(file);

        // Creo la estructura de archivo y le agrego la data.
        t_fileData* fileData = malloc(sizeof(t_fileData));
        fileData->firstBlockIndex = firstFreeBlockIndex;
        fileData->amountOfBlocks = 1;
        fileData->filePointer = 0;
        fileData->size = 0;
        fileData->metaData = config_create(fullFileName);
        char* firstFreeBlockIndexString = string_itoa(firstFreeBlockIndex);
        config_set_value(fileData->metaData, "BLOQUE_INICIAL", firstFreeBlockIndexString);
        char* sizeFileString = string_itoa(0);
        config_set_value(fileData->metaData, "TAMANIO_ARCHIVO", sizeFileString);
        config_save(fileData->metaData);

        // Agrego la estructura del archivo al diccionario que contiene todos los archivos.
        dictionary_put(fsData.files, params->fileName, fileData);

        free(fullFileName);
        free(firstFreeBlockIndexString);
        free(sizeFileString);
    }

    return success;
}

void executeIOFSDeleteAndSendResults()
{
    log_info(getLogger(), "PID: %d - Operacion: IO_FS_DELETE", (int)interfaceData.currentOperation.pid);
    
    delayFS();

    t_paramsForIOFSCreateOrDelete *params = (t_paramsForIOFSCreateOrDelete*)interfaceData.currentOperation.params;

    log_info(getLogger(), "PID: %d - Eliminar Archivo: %s", (int)interfaceData.currentOperation.pid, params->fileName);

    executeIOFSDelete();

    sendIOFSDeleteResultsToKernel();

    interfaceData.currentOperation.operation = IO_NULL;
    free(interfaceData.currentOperation.params);
    interfaceData.currentOperation.pid = -1;
}

void executeIOFSDelete()
{   
    t_paramsForIOFSCreateOrDelete *params = (t_paramsForIOFSCreateOrDelete*)interfaceData.currentOperation.params;
    
    t_fileData* fileData = (t_fileData*)dictionary_remove(fsData.files, params->fileName);

    for (int i = fileData->firstBlockIndex; i < fileData->firstBlockIndex + fileData->amountOfBlocks; i++)
    {
        bitarray_clean_bit(fsData.bitmap.bitmap, i);
    }

    msync(fsData.bitmap.mappedFile, fsData.bitmap.size, MS_SYNC);

    char* fullFileName = getFullFileName(params->fileName);
    config_destroy(fileData->metaData);
    remove(fullFileName);
    free(fileData);
    free(fullFileName);
}

void executeIOFSTruncateAndSendResults()
{
    log_info(getLogger(), "PID: %d - Operacion: IO_FS_TRUNCATE", (int)interfaceData.currentOperation.pid);

    delayFS();

    t_paramsForIOFSTruncate *params = (t_paramsForIOFSTruncate*)interfaceData.currentOperation.params;

    log_info(getLogger(), "PID: %d - Truncar Archivo: %s - Tamaño: %d", (int)interfaceData.currentOperation.pid, params->fileName, (int)params->size);

    executeIOFSTruncate();

    sendIOFSTruncateResultsToKernel();

    interfaceData.currentOperation.operation = IO_NULL;
    free(interfaceData.currentOperation.params);
    interfaceData.currentOperation.pid = -1;
}

void executeIOFSTruncate()
{
    t_paramsForIOFSTruncate *params = (t_paramsForIOFSTruncate*)interfaceData.currentOperation.params;
        
    char *nameForMetaDataFile = getFullFileName(params->fileName);

    t_fileData* fileData = dictionary_get(fsData.files, params->fileName);

    int newAmountOfBlocks = getAmountOfBlocks(params->size);


    // Si no hay que agregarle ni quitarle bloques, no hace nada, simplemente retorna porque se queda igual
    if (newAmountOfBlocks == fileData->amountOfBlocks)
    {
        fileData->size = params->size;
        char* sizeString = string_itoa(params->size);
        config_set_value(fileData->metaData, "TAMANIO_ARCHIVO", sizeString);
        config_save(fileData->metaData);
        free(sizeString);
        free(nameForMetaDataFile);

        return;
    }


    // Si solo hay que reducirle la cantidad de bloques, solamente pongo como libres los bloques del final que tengo que quitarle al archivo.
    if (newAmountOfBlocks < fileData->amountOfBlocks)
    {
        for (int i = fileData->firstBlockIndex + fileData->amountOfBlocks - 1; i >= fileData->firstBlockIndex + newAmountOfBlocks; i--)
        {
            bitarray_clean_bit(fsData.bitmap.bitmap, i);
        }
        
        msync(fsData.bitmap.mappedFile, fsData.bitmap.size, MS_SYNC);

        fileData->size = params->size;
        fileData->amountOfBlocks = newAmountOfBlocks;
        char* sizeString = string_itoa(params->size);
        config_set_value(fileData->metaData, "TAMANIO_ARCHIVO", sizeString);
        config_save(fileData->metaData);
        free(sizeString);
        free(nameForMetaDataFile);

        return;
    }


    ///////// A partir de aca, se sabe que se debe ampliar el archivo, es decir, asignarle más bloques contiguos.


    ///////// COMPRUEBO SI ES NECESARIO COMPACTAR PORQUE EL ARCHIVO ENTRA EN LOS BLOQUES CONTIGUOS

    bool needsCompacting = false;

    for (int i = fileData->firstBlockIndex + fileData->amountOfBlocks; i < fileData->firstBlockIndex + newAmountOfBlocks && !needsCompacting; i++)
    {
        needsCompacting = bitarray_test_bit(fsData.bitmap.bitmap, i);
    }
    
    
    // Si es necesario compactar los archivos porque no hay espacio para aumentarle el tamaño a uno, entonces lo compacta.
    if (needsCompacting)
    {
        compactAndSendFileToLast(params->fileName);
    }

    // En este punto ya me aseguro que el archivo tiene la maxima cantidad de bloques libres contiguos para aumentarle el tamaño ocupandolos.
    // No se tiene en cuenta si el archivo pide más bytes de los que le quedan libres al File System. En ese caso, el comportamiento es indefinido.

    for (int i = fileData->firstBlockIndex + fileData->amountOfBlocks; i < fileData->firstBlockIndex + newAmountOfBlocks; i++)
    {
        bitarray_set_bit(fsData.bitmap.bitmap, i);
    }
    
    msync(fsData.bitmap.mappedFile, fsData.bitmap.size, MS_SYNC);

    fileData->size = params->size;
    fileData->amountOfBlocks = newAmountOfBlocks;
    char* sizeString = string_itoa(params->size);
    config_set_value(fileData->metaData, "TAMANIO_ARCHIVO", sizeString);
    config_save(fileData->metaData);

    free(sizeString);
    free(nameForMetaDataFile);
}

void executeIOFSWriteAndSendResults()
{
    log_info(getLogger(), "PID: %d - Operacion: IO_FS_WRITE", (int)interfaceData.currentOperation.pid);

    delayFS();

    t_paramsForIOFSWriteOrRead *params = (t_paramsForIOFSWriteOrRead*)interfaceData.currentOperation.params;

    log_info(getLogger(), "PID: %d - Escribir Archivo: %s - Tamaño a Escribir: %d - Puntero Archivo: %d", (int)interfaceData.currentOperation.pid, params->fileName, (int)params->totalSize, (int)params->filePointer);

    executeIOFSWrite();

    sendIOFSWriteResultsToKernel();

    free(resultsForIOFSWrite.resultsForWrite);
    resultsForIOFSWrite.resultsForWrite = NULL;

    free(params->addressesInfo);
    interfaceData.currentOperation.operation = IO_NULL;
    free(interfaceData.currentOperation.params);
    interfaceData.currentOperation.pid = -1;
}

void executeIOFSWrite()
{
    t_paramsForIOFSWriteOrRead *params = (t_paramsForIOFSWriteOrRead*)interfaceData.currentOperation.params;

    resultsForIOFSWrite.resultsForWrite = (char*)readFromMemory(params->addressesInfo, params->amountOfPhysicalAddresses, params->totalSize);
    
    //Se espera a recibir el contenido de la memoria
    //sem_wait(&semaphoreReceiveDataFromMemory);

    t_fileData* fileData = (t_fileData*)dictionary_get(fsData.files, params->fileName);
    char* fullFileName = getFullFileName(params->fileName);

    int fileOffset = getFirstByteOfBlock(fileData->firstBlockIndex);
    memcpy(fsData.blocks.mappedFile + fileOffset + params->filePointer, resultsForIOFSWrite.resultsForWrite, params->totalSize);
    msync(fsData.blocks.mappedFile, fsData.blocks.size, MS_SYNC);
    free(fullFileName);
}

void executeIOFSReadAndSendResults()
{
    log_info(getLogger(), "PID: %d - Operacion: IO_FS_READ", (int)interfaceData.currentOperation.pid);

    delayFS();

    t_paramsForIOFSWriteOrRead *params = (t_paramsForIOFSWriteOrRead*)interfaceData.currentOperation.params;

    log_info(getLogger(), "PID: %d - Leer Archivo: %s - Tamaño a Leer: %d - Puntero Archivo: %d", (int)interfaceData.currentOperation.pid, params->fileName, (int)params->totalSize, (int)params->filePointer);

    resultsForIOFSRead.resultsFromRead = malloc(params->totalSize);

    executeIOFSRead();



    writeToMemory(resultsForIOFSRead.resultsFromRead, params->addressesInfo, params->amountOfPhysicalAddresses);

    // Se espera a recibir confirmación de la memoria de que salió todo bien
    //sem_wait(&semaphoreForIOFSRead);

    sendIOFSReadResultsToKernel();

    free(resultsForIOFSRead.resultsFromRead);
    resultsForIOFSRead.resultsFromRead = NULL;

    free(params->addressesInfo);
    interfaceData.currentOperation.operation = IO_NULL;
    free(interfaceData.currentOperation.params);
    interfaceData.currentOperation.pid = -1;
}

void executeIOFSRead()
{
    t_paramsForIOFSWriteOrRead *params = (t_paramsForIOFSWriteOrRead*)interfaceData.currentOperation.params;
    //char *fullFileName = getFullFileName(params->fileName);

    t_fileData* fileData = dictionary_get(fsData.files, params->fileName);

    int fileOffset = getFirstByteOfBlock(fileData->firstBlockIndex);
    memcpy(resultsForIOFSRead.resultsFromRead, fsData.blocks.mappedFile + fileOffset + params->filePointer, params->totalSize);
}

bool takeFirstFreeBlock(int* blockIndex)
{
    *blockIndex = 0;

    // Se fija cual es el primer bloque libre.
    while(*blockIndex < getIOConfig()->BLOCK_COUNT && bitarray_test_bit(fsData.bitmap.bitmap, *blockIndex) != 0)
    {
        (*blockIndex)++;
    }


    // Caso de error, no hay ningun bloque libre para reservar.
    if (*blockIndex >= getIOConfig()->BLOCK_COUNT)
    {
        return false;
    }


    // Caso de exito. Hay un bloque libre para reservar. A continuacion se marca como ocupado ese bloque, para reservarlo.

    bitarray_set_bit(fsData.bitmap.bitmap, *blockIndex);
    msync(fsData.bitmap.mappedFile, fsData.bitmap.size, MS_SYNC);

    return true;
}



// Funcion auxiliar para comparar cuál archivo deberia ir primero en una lista. Sirve para ordenar la lista de archivos.
bool comparator(void* element1, void* element2)
{
    t_fileData* fileData1 = (t_fileData*)element1;
    t_fileData* fileData2 = (t_fileData*)element2;

    // Se ordena segun lo siguiente: el primero de la lista es el que tiene el bloque mas al principio del archivo de bloques, y el ultimo es el que tiene el bloque mas al final del archivo de bloques
    return fileData1->firstBlockIndex < fileData2->firstBlockIndex; //////////////// FIJARSE SI ESTÁ BIEN, QUIZAS DEBA SER CON EL SIGNO AL REVES (>).
}

void compactAndSendFileToLast(char* fileNameToSendToLast)
{
    log_info(getLogger(), "PID: %d - Inicio Compactación.", (int)interfaceData.currentOperation.pid);

    delayCompacting();

    // Obtengo el archivo que voy a mandar al final como ultimo archivo.
    t_fileData* fileDataToSendToLast = dictionary_get(fsData.files, fileNameToSendToLast);

    // Copio los bytes del archivo en una variable interna.
    void* fileBytesToSendToLast = malloc(fileDataToSendToLast->size);
    memcpy(fileBytesToSendToLast, fsData.blocks.mappedFile + getFirstByteOfBlock(fileDataToSendToLast->firstBlockIndex), fileDataToSendToLast->size);

    // Uso una lista para poder ordenarla y poder compactar los archivos desde el principio hasta el final, segun como estan puestos en los bloques.dat del FS.
    t_list* filesList = dictionary_elements(fsData.files);
    list_remove_element(filesList, (void*)fileDataToSendToLast);

    // Se ordena segun lo siguiente: el primero de la lista es el que tiene el bloque mas al principio del archivo de bloques, 
    // y el ultimo es el que tiene el bloque mas al final del archivo de bloques
    list_sort(filesList, comparator);


    // Compacto todos los archivos.

    t_list_iterator* iterator = list_iterator_create(filesList);

    int blocksOffset = 0;
    while (list_iterator_has_next(iterator))
    {
        // Obtengo el elemento actual
        t_fileData* fileElement = (t_fileData*)list_iterator_next(iterator);
        
        // "Corro" los bytes del archivo hacia lo más al principio posible sin pisarse con otros archivos, así compactandolos.
        void* fileBytesElement = malloc(fileElement->size);
        memcpy(fileBytesElement, fsData.blocks.mappedFile + getFirstByteOfBlock(fileElement->firstBlockIndex), fileElement->size);
        memcpy(fsData.blocks.mappedFile + getFirstByteOfBlock(blocksOffset), fileBytesElement, fileElement->size);

        // Actualizo la informacion del archivo
        fileElement->firstBlockIndex = blocksOffset;
        char* blockOffsetString = string_itoa(blocksOffset);
        config_set_value(fileElement->metaData, "BLOQUE_INICIAL", blockOffsetString);
        config_save(fileElement->metaData);

        // El blocksOffset es el primer bloque del siguiente archivo de la iteracion.
        blocksOffset += fileElement->amountOfBlocks;
        free(fileBytesElement);
        free(blockOffsetString);
    }

    list_iterator_destroy(iterator);

    // Pongo al final el archivo que queria poner al final.
    memcpy(fsData.blocks.mappedFile + getFirstByteOfBlock(blocksOffset), fileBytesToSendToLast, fileDataToSendToLast->size);

    // Actualizo la informacion del archivo.
    fileDataToSendToLast->firstBlockIndex = blocksOffset;
    char* blockOffsetString = string_itoa(blocksOffset);
    config_set_value(fileDataToSendToLast->metaData, "BLOQUE_INICIAL", blockOffsetString);
    config_save(fileDataToSendToLast->metaData);

    msync(fsData.blocks.mappedFile, fsData.blocks.size, MS_SYNC);

    free(fileBytesToSendToLast);
    free(blockOffsetString);



    // Actualizo el bitmap.

    for (int i = 0; i < fileDataToSendToLast->firstBlockIndex + fileDataToSendToLast->amountOfBlocks; i++)
    {
        bitarray_set_bit(fsData.bitmap.bitmap, i);
    }
    
    msync(fsData.bitmap.mappedFile, fsData.bitmap.size, MS_SYNC);

    list_destroy(filesList);

    log_info(getLogger(), "PID: %d - Fin Compactación.", (int)interfaceData.currentOperation.pid);
}







//////////////////////// FUNCIONES AUXILIARES ////////////////////////



bool isFileInFS(char* fileName)
{
    return strcmp(fileName, "bloques.dat") != 0 && strcmp(fileName, "bitmap.dat") != 0;
}