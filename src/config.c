#include "config.h"
#include "log.h"

void init_config(struct config* config)
{
    int error = ini_parse("./autoc.ini", handler, config);
    if (error == -1)
    {
        flog(LOG_ERROR, "Unimplemented: create default config if doesnt exist");
        exit(1);
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

// void init_config(struct config* config)
// {
//     const char* const filename = "./.autoc";
//
//     int fd = open(filename, O_RDWR);
//     if (fd < 0)
//     {
//         flog(LOG_ERROR, "Failed to open '%s': %s", filename, strerror(errno));
//         goto skip_file;
//     }
//     struct stat st;
//     if (fstat(fd, &st))
//     {
//         flog(LOG_ERROR, "Failed to stat '%s': %s", filename, strerror(errno));
//         goto skip_file;
//     }   
//
//     char* file = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//     
//     parse_config(config, file);
//
//     munmap(file, st.st_size);
//     
//     return;
//
// skip_file:
//     flog(LOG_ERROR, "unimplemented: self input");
//     exit(0);
//
// }
//
//
// void parse_config(struct config* config, const char* file)
// {
//     char* file_copy = strdup(file);
//     if (!file_copy)
//     {
//         fprintf(stderr, "Failed to allocate memory\n");
//         return;
//     }
//
//     char* line = strtok(file_copy, "\n");
//     while (line) 
//     {
//         while (isspace(*line))
//         {
//             line++;
//         }
//
//         if (!strncmp(line, "src", 3))
//         {
//             line += 3;
//             while (isspace(*line))
//             {
//                 line++;
//             }
//             flog(LOG_INFO, "Setting src_dir");
//             config->src_dir = line;
//         } 
//         else if (!strncmp(line, "bin", 3))
//         {
//             line += 3;
//             while (isspace(*line))
//             {
//                 line++;
//             }
//             flog(LOG_INFO, "Setting bin_dir");
//             config->bin_dir = line;
//         } 
//         else if (!strncmp(line, "target", 6))
//         {
//             line += 6;
//             while (isspace(*line))
//             {
//                 line++;
//             }
//             flog(LOG_INFO, "Setting target");
//             config->target = line;
//         } 
//
//         while (!isspace(*line))
//         {
//             line++;
//         }
//
//         char* new_line = strtok(NULL, "\n");
//         *line = 0;
//         line = new_line;
//     }
// }




