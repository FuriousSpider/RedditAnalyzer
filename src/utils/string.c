#include "redditanalyzer/utils/string.h"

#include <stdlib.h>
#include <string.h>

char *ra_strdup(const char *source)
{
    if (source == NULL)
    {
        return NULL;
    }

    const size_t length = strlen(source);

    char *copy = malloc(length + 1U);

    if (copy == NULL)
    {
        return NULL;
    }

    memcpy(copy, source, length + 1U);
    return copy;
}
