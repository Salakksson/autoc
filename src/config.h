#ifndef CONFIG_H_
#define CONFIG_H_

#include "ini.h"

struct config
{
    const char* bin_dir;
    const char* src_dir;

    const char* target;


};

void init_config(struct config* config);

int handler(void* user, const char* section, const char* name, const char* value);

// void parse_config(struct config* config, const char* file);


#endif 
