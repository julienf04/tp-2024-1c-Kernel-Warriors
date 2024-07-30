#ifndef IO_CLIENT_H
#define IO_CLIENT_H

#include "serverIO.h"
#include "utils/client/utils.h"

void sendInterfaceToKernel();

void sendIOGenSleepResultsToKernel();

void sendIOStdinReadResultsToKernel();

void sendResultsFromStdinToMemory(void* data, uint32_t physicalDirection, uint32_t size);

void sendResultsFromIOFSReadToMemory();

void sendIOStdoutWriteResultsToKernel();

void sendIOReadRequestToMemory(uint32_t physicalDirection, uint32_t size);

void sendIOFSCreateResultsToKernel(int sucess);

void sendIOFSDeleteResultsToKernel();

void sendIOFSTruncateResultsToKernel();

void sendIOFSWriteResultsToKernel();

void sendIOFSReadResultsToKernel();

#endif