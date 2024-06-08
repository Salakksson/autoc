#include "log.h"
#include "compile.h"
#include "config.h"

/* Features to add
* - Support for multiple directories
* - Support for precompiled headers
* - Dont recompile unchanged files
*/
int main(int argc, char** argv)
{
    struct config conf;
    init_config(&conf);
    const char** ls = get_directory_list(conf.src_dir);
    for (int i = 0; ls[i]; i++)
    {
        // flog(LOG_INFO, "Compiling file '%s'", ls[i]);
        if(compile(&conf, ls[i], conf.bin_dir))
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

