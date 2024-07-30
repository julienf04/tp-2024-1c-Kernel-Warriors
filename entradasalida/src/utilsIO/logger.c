#include "logger.h"

t_log* _logger;


void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level)
{
    _logger = log_create(file, processName, isActiveConsole, level);
}

t_log* getLogger()
{
    return _logger;
}

void destroyLogger()
{
    log_destroy(_logger);
}