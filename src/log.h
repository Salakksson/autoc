#ifndef LOG
#define LOG

#define COLOUR_RED     "\x1b[31m"
#define COLOUR_YELLOW  "\x1b[33m"
#define COLOUR_BLUE    "\x1b[34m"
#define COLOUR_MAGENTA "\x1b[35m"

#define COLOUR_RESET   "\x1b[0m"

#include <helper>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

typedef enum 
{
    LOG_FATAL,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG
}log_type;


void flog(log_type type, const char* message, ...);


void* readf_alloc(const char* filepath);

#endif
