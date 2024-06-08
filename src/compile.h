#ifndef COMPILE_H_
#define COMPILE_H_

#include "log.h"
#include "config.h"

char* format_string(const char* fmt, const char* source, const char* output);


const char* get_extension(const char* file);

void run_command(const char** args);

int compile(struct config* conf, const char* source, const char* bin_dir);

const char** get_directory_list(const char* dir);

int link_to_target(struct config* conf);

#endif // !COMPILE_H_
