#include "delay.h"

#include "config.h"
#include <unistd.h>

void memoryDelay()
{
    usleep( (useconds_t) getMemoryConfig()->RETARDO_RESPUESTA * 1000 );
}