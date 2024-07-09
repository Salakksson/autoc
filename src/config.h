#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdbool.h>

#include "ini.h"
#include "hashmap.h"

struct config
{
    char* bin_dir;
    char* src_dir;
    
    char* ldflags;
    char* target;

    hashmap commands;

    bool force_compile;
    bool link_required;
    bool twice;
    bool quiet;
    bool run_target;
    int clear_bin;
};

void init_config(struct config* config);

int handler(void* user, const char* section, const char* name, const char* value);

void create_config(const char* path);

#endif 
