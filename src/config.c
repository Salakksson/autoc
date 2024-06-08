#include "config.h"
#include "log.h"

void init_config(struct config* config)
{
    config->commands = init_hashmap(20);
    int error = ini_parse("./autoc.ini", handler, config);
    if (error == -1)
    {
        create_config("./autoc.ini");
        exit(0);
    }
    else if (error != 0)
    {
        flog(LOG_ERROR, "Failed to parse autoc.ini: ERROR on line %d", error);
        exit(1);
    }
    error = false;
    if (!config->target)
    {
        flog(LOG_ERROR, "No target specified in config");
        error = true;
    }
    if (!config->src_dir)
    {
        flog(LOG_ERROR, "No source directory specified in config");
        error = true;
    }
    if (!config->bin_dir)
    {
        flog(LOG_ERROR, "No binary directory specified in config");
        error = true;
    }
    if (error) exit(1);
}

struct mapping 
{
    const char *name;
    char **field;
};

int handler(void* user, const char* section, const char* name, const char* value)
{
    struct config* config = user;
    
    if(*section == '.')
    {
        // flog(LOG_INFO, "parsing section %s, name %s, value: %s", section, name, value);
        if (strcmp(name, "command")) return 1;
        
        hset(&config->commands, section, value);
        return 1;
    }

    struct mapping map[] = 
    {
        {"src", &config->src_dir},
        {"bin", &config->bin_dir},
        {"ldflags", &config->ldflags},
        {"target", &config->target}
    };
    const size_t map_size = sizeof(map)/sizeof(*map);
    for (size_t i = 0; i < map_size;)
    {
        if (!strcmp(name, map[i].name)) 
        {
            *map[i].field = strdup(value);
            break;
        }
        if (++i == map_size)
        {
            flog(LOG_ERROR, "Unknown field in config: [%s] %s = %s", section, name, value);
        }
    }
    return 1;
}

#include "default_config.h"

void create_config(const char* path)
{
    flog(LOG_INFO, "Creating default config '%s', modify it before rerunning", path);

    int fd = creat(path, 0666);
    if (fd == -1)
    {
        flog(LOG_ERROR, "Failed to create default config, exiting");
        exit(1);
    }
    int err = write(fd, default_config, sizeof(default_config));
    if (err < sizeof(default_config))
    {
        flog(LOG_ERROR, "Failed to write default config '%s', check if it looks valid");
        exit(1);
    }
}
