#include "log.h"


void flog(log_type type, const char* message, ...)
{
    va_list args;
    va_start(args, message);
    
    const char* prefix;
    const char* colour;

    switch (type)
    {
        case LOG_ERROR:
            prefix = "ERROR: ";
            colour = COLOUR_RED;
            break;
        case LOG_WARNING:
            prefix = "Warning: ";
            colour = COLOUR_YELLOW;
            break;
        case LOG_INFO:
            prefix = "Info: ";
            colour = COLOUR_BLUE;
            break;
        case LOG_DEBUG:
            prefix = "Debug: ";
            colour = COLOUR_MAGENTA;
            break;
        default:
            prefix = "";
            colour = COLOUR_RESET;
            break;
    }

    printf("%s%s", colour, prefix);
    vprintf(message, args);
    printf("%s\n", COLOUR_RESET);

    va_end(args);
}

void* readf_alloc(const char* filepath)
{
    int fd = open(filepath, O_RDONLY);
    if (fd < 0)
    {
        flog(LOG_ERROR, "open: %s", strerror(errno));
        exit(1);
    }

    struct stat st;
    if (fstat(fd, &st) < 0)
    {
        flog(LOG_ERROR, "fstat: %s", strerror(errno));
        exit(1);
    }
    
    void* buffer = calloc(st.st_size+1, 1);
    if (!buffer)
    {
        flog(LOG_ERROR, "calloc failed, ur running on a potato: %s", strerror(errno));
        exit(1);
    }

    if (read(fd, buffer, st.st_size) != st.st_size)
    {
        flog(LOG_ERROR, "read: %s", strerror(errno));
        exit(1);
    }
    close(fd);

    return buffer;
}
