#include "redditanalyzer/utils/error.h"

const char *ra_error_string(RaError error)
{
    switch (error)
    {
        case RA_OK:
            return "Success";

        case RA_ERR_INVALID_ARGUMENT:
            return "Invalid argument";

        case RA_ERR_OUT_OF_MEMORY:
            return "Out of memory";

        case RA_ERR_NETWORK:
            return "Network error";

        case RA_ERR_TIMEOUT:
            return "Timeout";

        case RA_ERR_HTTP:
            return "HTTP error";

        case RA_ERR_RATE_LIMIT:
            return "Rate limit exceeded";

        case RA_ERR_JSON:
            return "JSON error";

        case RA_ERR_NOT_FOUND:
            return "Not found";

        case RA_ERR_DATABASE:
            return "Database error";

        case RA_ERR_INTERNAL:
            return "Internal error";

        default:
            return "Unknown error";
    }
}