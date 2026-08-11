#include "redditanalyzer/utils/logger.h"

#include <stdio.h>

void ra_log_info(const char *message)
{
    fprintf(stdout, "[INFO] %s\n", message);
}

void ra_log_warning(const char *message)
{
    fprintf(stderr, "[WARNING] %s\n", message);
}

void ra_log_error(const char *message)
{
    fprintf(stderr, "[ERROR] %s\n", message);
}

void ra_log_debug(const char *message)
{
    fprintf(stdout, "[DEBUG] %s\n", message);
}
