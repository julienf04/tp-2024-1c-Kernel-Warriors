#ifndef GENERIC_INTERFACE_H
#define GENERIC_INTERFACE_H

#include "connections/serverIO.h"
#include "connections/clientIO.h"
#include "utilsIO/logger.h"
#include "interfaces.h"

#include <unistd.h>

void executeIOGenSleepAndSendResults();

void executeIOGenSleep();

#endif