#include "mathMemory.h"



int getInternalFragmentation(int pageSize, int bytesAllocated)
{
    return (  pageSize - (  bytesAllocated % pageSize  )  ) % pageSize;
}

int getAmountOfBytesInTheLastPage(int pageSize, int internalFragmentation)
{
    return pageSize - internalFragmentation;
}

int getAmountOfBytesAllocated(int pageSize, int amountOfPages, int internalFragmentation)
{
    return amountOfPages * pageSize - internalFragmentation;
}

int getAmountOfPagesAllocated(int pageSize, int bytesAllocated)
{
    return ceil((float)bytesAllocated / (float)pageSize);
}

int getAmountOfPagesToFree(int pageSize, int bytesToFree)
{
    return floor((float)bytesToFree / (float)pageSize) + 1;
}

int calculatePhysicalAddr(int pageSize, int frame, int offset)
{
    return frame * pageSize + offset;
}