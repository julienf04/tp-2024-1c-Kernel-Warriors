#include "essentials.h"



listMutex_t* processesList;




processInfo* lastProcess;



processInfo* getLastProcess()
{
    return lastProcess;
}



void setLastProcess(processInfo* value)
{
    lastProcess = value;
}




sem_t semAuxPID;


int auxPID;

bool closurePIDsAreEqual(void* element)
{
    return ((processInfo*)element)->PID == auxPID;
}



