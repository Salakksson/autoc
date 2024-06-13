#include "log.h"
#include "compile.h"
#include "config.h"

time_t file_mod_time(const char* path)
{
    struct stat attr;
    if (stat(path, &attr) == -1)
    {
        if (errno == ENOENT) return 0;

        flog(LOG_ERROR, "Failed to stat '%s'", path);
        exit(1);
    }
    return attr.st_mtime;
}
 
/* Features to add
* - Support for multiple directories
* - Support for precompiled headers
* - Custom linker commands
*/
int main(int argc, char** argv)
{
    struct config conf = {0};
    init_config(&conf);
    if (argc > 1)
    {
        if (*argv[1] == '-')
        switch(argv[1][1])
        {
        case 'f':
            conf.force_compile = true;
            break;
        case 'h':
            flog(LOG_INFO, "Usage: %s <flags>", *argv);
            flog(LOG_INFO, "-f: force compile all modules");
            flog(LOG_INFO, "-h: show this info");
            exit(0);
        }
    }
    time_t config_time = file_mod_time("./autoc.ini");

    const char** ls = get_directory_list(conf.src_dir);
    for (int i = 0; ls[i]; i++)
    {
        // flog(LOG_INFO, "Compiling file '%s'", ls[i]);
        const char* src = ls[i];
        char* bin = get_binary_from_source(src, conf.bin_dir); 
        
        time_t src_time = file_mod_time(src);
        time_t bin_time = file_mod_time(bin);

        if (conf.force_compile || MAX(src_time, config_time) > bin_time)
        if (compile(&conf, ls[i], conf.bin_dir))
        {
            flog(LOG_ERROR, "Failed to compile '%s'", ls[i]);
            exit(1);
        }
    }
    if (link_to_target(&conf))
    {
        flog(LOG_ERROR, "Failed to link '%s'", conf.target);
        exit(1);
    }
    return 0;
}

