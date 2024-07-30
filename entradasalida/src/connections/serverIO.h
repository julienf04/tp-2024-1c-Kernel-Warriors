#ifndef IO_SERVER_H
#define IO_SERVER_H

#include "clientIO.h"
#include "genericInterface.h"
#include "stdinInterface.h"
#include "stdoutInterface.h"
#include "dialfsInterface.h"
#include "interfaces.h"
#include "utils/server/utils.h"
#include "utils/utilsGeneral.h"
#include "utilsIO/logger.h"

void serverIOForKernel(int *socketClient);

void serverIOForMemory(int *socketClient);

void sendResultsFromIOGenSleepToKernel();

void sendResultsFromIOStdinReadToKernel();

void sendResultsFromIOStdoutWriteToKernel();

void sendResultsFromIOFSCreateToKernel();

void sendResultsFromIOFSDeleteToKernel();

void sendResultsFromIOFSTruncateToKernel();

void sendResultsFromIOFSWriteToKernel();

void sendResultsFromIOFSReadToKernel();

void receiveDataFromMemory();

void finishAllServersSignal();

#endif