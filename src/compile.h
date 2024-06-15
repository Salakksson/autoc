#ifndef COMPILE_H_
#define COMPILE_H_

#include <time.h>

#include "log.h"
#include "config.h"

time_t file_mod_time(const char* path);

char* get_binary_from_source(const char* src, const char* bin_dir);

int compile(struct config* conf, const char* source, const char* bin_dir);

const char** get_directory_list(const char* dir);

int link_to_target(struct config* conf);

#endif // !COMPILE_H_
