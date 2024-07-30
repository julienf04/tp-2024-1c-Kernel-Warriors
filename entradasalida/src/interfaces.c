#include "interfaces.h"
#include "math.h"

t_interfaceData interfaceData;
t_resultsForStdin resultsForStdin;
t_resultsForStdout resultsForStdout;
t_resultsForIOFSWrite resultsForIOFSWrite;
t_resultsForIOFSRead resultsForIOFSRead;

sem_t semaphoreSendDataToMemory;
sem_t semaphoreReceiveDataFromMemory;
sem_t semaphoreForModule;

t_FSData fsData;

int socketKernel;
int socketMemory;

void createInterface(char *name)
{   
    interfaceData.name = malloc(string_length(name) + 1);
    strcpy(interfaceData.name, name);

    if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "GENERICA"))
        interfaceData.type = Generic;
    else if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "STDIN"))
        interfaceData.type = STDIN;
    else if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "STDOUT"))
        interfaceData.type = STDOUT;
    else if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "DIALFS"))
        interfaceData.type = DialFS;

    interfaceData.workUnits = getIOConfig()->TIEMPO_UNIDAD_TRABAJO;

    interfaceData.currentOperation.pid = -1;
    interfaceData.currentOperation.operation = IO_NULL;
    switch (interfaceData.type)
    {
        case Generic:
            interfaceData.currentOperation.params = malloc(sizeof(t_paramsForGenericInterface));
            break;

        case STDIN:
            interfaceData.currentOperation.params = malloc(sizeof(t_paramsForStdinInterface));
            resultsForStdin.resultsFromRead = NULL;
            resultsForStdin.resultsForMemory = NULL;
            break;

        case STDOUT:
            interfaceData.currentOperation.params = malloc(sizeof(t_paramsForStdoutInterface));
            resultsForStdout.resultsForWrite = NULL;
            break;

        case DialFS:
            const char *dir = getIOConfig()->PATH_BASE_DIALFS;
            mkdir(dir, 0777);

            int sizeBlocksFile = getIOConfig()->BLOCK_COUNT * getIOConfig()->BLOCK_SIZE;
            fsData.blocks.mappedFile = openCreateMapFile(&(fsData.blocks.file), "bloques.dat", sizeBlocksFile);
            fsData.blocks.size = sizeBlocksFile;

            int sizeBitmapFile = ceil((double)getIOConfig()->BLOCK_COUNT / 8);
            fsData.bitmap.mappedFile = openCreateMapFile(&(fsData.bitmap.file), "bitmap.dat", sizeBitmapFile);
            fsData.bitmap.bitmap = bitarray_create_with_mode(fsData.bitmap.mappedFile, sizeBitmapFile, LSB_FIRST);
            fsData.bitmap.size = sizeBitmapFile;
            //for (int i = 0; i < fsData.bitmap.size * 8; i++) // * 8 porque 1 byte son 8 bits
            //{
            //    bitarray_clean_bit(fsData.bitmap.bitmap, i);
            //}
            
            createDictionaryFileNames();

            break;

        default:
        break;
    }
}

void destroyInterface()
{
    free(interfaceData.name);
    free(interfaceData.currentOperation.params);
}

void* openCreateMapFile(FILE** file, char* fileName, int fileSize)
{   
    char *fullName = getFullFileName(fileName);
    *file = fopen(fullName, "ab+");

    int fd = fileno(*file);

    ftruncate(fd, fileSize);

    void *mappedFile;
    mappedFile = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    msync(mappedFile, fileSize, MS_SYNC);

    free(fullName);
    return mappedFile;
}

void createDictionaryFileNames()
{
        fsData.files = dictionary_create();
        DIR* dir;
        struct dirent* entry;

        dir = opendir(getIOConfig()->PATH_BASE_DIALFS);

        while ((entry = readdir(dir)) != NULL)
        {   
            if (entry->d_type == DT_REG && isFileInFS(entry->d_name))
            {
                char* fullFileName = getFullFileName(entry->d_name);
                t_fileData* fileData = malloc(sizeof(t_fileData));
                fileData->metaData = config_create(fullFileName);
                fileData->firstBlockIndex = config_get_int_value(fileData->metaData, "BLOQUE_INICIAL");
                fileData->size = config_get_int_value(fileData->metaData, "TAMANIO_ARCHIVO");
                fileData->amountOfBlocks = ceil( (double)fileData->size / (double)getIOConfig()->BLOCK_SIZE );
                fileData->filePointer = 0;

                dictionary_put(fsData.files, entry->d_name, fileData);

                free(fullFileName);
            }
        }

        closedir(dir);
}

void closeBlocksFile()
{   
    if (fsData.blocks.file != NULL)
    {
        msync(fsData.blocks.mappedFile, fsData.blocks.size, MS_SYNC);
        munmap(fsData.blocks.mappedFile, fsData.blocks.size);
        fclose(fsData.blocks.file);
    }
}

void closeBitmapFile()
{   
    if (fsData.bitmap.file != NULL)
    {   
        msync(fsData.bitmap.mappedFile, fsData.bitmap.size, MS_SYNC);
        munmap(fsData.bitmap.mappedFile, fsData.bitmap.size);
        fclose(fsData.bitmap.file);
        bitarray_destroy(fsData.bitmap.bitmap);
    }
}

void closeSingleFileClosure(char* key, void* value)
{
    t_fileData* fileData = (t_fileData*)value;

    config_save(fileData->metaData);
    config_destroy(fileData->metaData);
    free(value);
}

void closeAllFiles()
{
    dictionary_iterator(fsData.files, closeSingleFileClosure);
    dictionary_destroy(fsData.files);
}





/////////////////////////////// FUNCIONES AUXILIARES ///////////////////////////////



void writeToMemory(void* data, physicalAddressInfo* addressesInfo, int amountOfPhysicalAddresses)
{
    t_paramsForStdinInterface *params = (t_paramsForStdinInterface*)interfaceData.currentOperation.params;
    int offset = 0;

    for (int i = 0; i < amountOfPhysicalAddresses; i++)
    {
        sendResultsFromStdinToMemory(data + offset, addressesInfo[i].physicalAddress, addressesInfo[i].size);
        offset += addressesInfo[i].size;
        sem_wait(&semaphoreSendDataToMemory);
    }
}

void* dataReceivedFromMemory;

void* readFromMemory(physicalAddressInfo* addressesInfo, int amountOfPhysicalAddresses, int size)
{
    void* data = malloc(size);
    int offset = 0;
    for (int i = 0; i < amountOfPhysicalAddresses; i++)
    {
        sendIOReadRequestToMemory(addressesInfo[i].physicalAddress, addressesInfo[i].size);
        sem_wait(&semaphoreReceiveDataFromMemory);
        memcpy(data + offset, dataReceivedFromMemory, addressesInfo[i].size);
        offset += addressesInfo[i].size;
        free(dataReceivedFromMemory);
    }

    return data;
}


char* getFullFileName(char* fileName)
{
    char *fullName = string_new();
    string_append(&fullName, getIOConfig()->PATH_BASE_DIALFS);
    string_append(&fullName, "/");
    string_append(&fullName, fileName);

    return fullName;
}


int getFirstByteOfBlock(int blockIndex)
{
    return blockIndex * getIOConfig()->BLOCK_SIZE;
}


int getAmountOfBlocks(int fileSize)
{
    return ceil((double)fileSize / (double)getIOConfig()->BLOCK_SIZE);
}

void delayCompacting()
{
    usleep(getIOConfig()->RETRASO_COMPACTACION * 1000);
}

void delayFS()
{
    usleep(getIOConfig()->TIEMPO_UNIDAD_TRABAJO * 1000);
}