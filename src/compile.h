#ifndef COMPILE_H_
#define COMPILE_H_

#include "log.h"

void run_command(const char** args);

int compile(const char* file, const char* bin_dir, bool forced);

const char** get_directory_list(const char* dir);

int link_to_target(const char* dir, const char* target);

#endif // !COMPILE_H_
