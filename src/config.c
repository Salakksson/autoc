#include "config.h"
#include "log.h"

void init_config(struct config* config)
{
    int error = ini_parse("./autoc.ini", handler, config);
    if (error == -1)
    {
        create_config("./autoc.ini");
        init_config(config);
        return;
    }
    else if (error != 0)
    {
        flog(LOG_ERROR, "Failed to parse autoc.ini: ERROR on line %d", error);
        exit(1);
    }
}

struct mapping 
{
    const char *name;
    const char **field;
};

int handler(void* user, const char* section, const char* name, const char* value)
{
    struct config* config = user;
    
    // flog(LOG_INFO, "Handling section '%s' name '%s' value '%s'", section, name, value);
    struct mapping map[] = 
    {
        {"src", &config->src_dir},
        {"bin", &config->bin_dir},
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
            flog(LOG_ERROR, "Failed to parse .autoc, unknown field '%s'", name);
        }
    }
    return 1;
}

#include "default_config.h"

void create_config(const char* path)
{
    flog(LOG_INFO, "Creating default config '%s'", path);

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
