#include "compile.h"

#include <string.h>
#include <sys/wait.h>
#include <libgen.h>
#include <assert.h>
#include <dirent.h>
#include <ctype.h>

void run_command(const char** argv)
{
    int child_pid = fork();

    if (child_pid == 0)
    {
        execvp(argv[0], (char**)argv);

        flog(LOG_ERROR, "execve: %s", strerror(errno));
        exit(1);
    }
    if (child_pid < 0)
    {
        flog(LOG_ERROR, "cannot fork process: %s", strerror(errno));
        exit(1);
    } 

    // flog(LOG_INFO, "Created child process '%s' with pid %d\n", argv[0], child_pid); 

    if (child_pid != waitpid(child_pid, 0, 0))
    {
        flog(LOG_ERROR, "waitpid: %s", strerror(errno));
        exit(1);
    }

}

int compile(const char* input, const char* bin_dir, bool forced)
{
    if(input[strlen(input) - 1] != 'c') return 0; 
    
    if(!*bin_dir)
    {
        flog(LOG_ERROR, "No binary directory set");
        exit(1);
    }
    size_t path_length = strlen(input) + strlen(bin_dir) + 5; // allocate enough memory for the final path
    char* out_path = malloc(path_length);                     // although the path will be shorter
    char* buffer = malloc(strlen(input));
    assert(out_path && buffer);

    strcpy(buffer, input);
    char* name = basename(buffer);

    char command[1024];
    int bytes = snprintf(command, sizeof(command), "gcc -c %s -o %s/%s.o", input, bin_dir, name);
    if (bytes < 14)
    {
        flog(LOG_ERROR, "failed to create compile command: %s", strerror(errno));
        flog(LOG_INFO, "print command? (might segfault) (y/N)");
        char choice = getchar();
        while (getchar() != '\n');
        if (toupper(choice) != 'Y')
            exit(1);
        
        printf("command: '%s'\n", command);
        flog(LOG_INFO, "continue? (y/N)");
        
        choice = getchar();
        while (getchar() != '\n');
        if (toupper(choice) != 'Y')
            exit(1);

        flog(LOG_INFO, "continuing... (you may have a deathwish)");
        sleep(1);
    }
    flog(LOG_INFO, "/bin/sh: %s", command);
    if (system(command))
    {
        return 1;
    }
    // snprintf(out_path, path_length, "%s/%s.o", bin_dir, name);
    // 
    // const char* command[] = {"gcc", input, "-c", "-o", out_path, NULL};
    // run_command(command);

    free(buffer);
    free(out_path);
    return 0;
}

const char** get_directory_list(const char* dir)
{
    DIR* d = opendir(dir);
    if (!d)
    {
        flog(LOG_ERROR, "failed to open directory '%s': %s", dir, strerror(errno));
        exit(1);
    }
    struct dirent* entry;
    int count = 0;
    int capacity = 16;

    const char** files = malloc(capacity * sizeof(char*));

    while((entry = readdir(d)))
    {
        if (entry->d_type == DT_REG)
        {
            char* filepath = malloc(strlen(dir) + strlen(entry->d_name) + 2);
            assert(filepath);
            sprintf(filepath, "%s/%s", dir, entry->d_name);

            files[count++] = filepath;
            if(count == capacity)
            {
                capacity *=2;
                files = realloc(files, capacity * sizeof(char*));
                assert(files);
            }
        }
    }
    closedir(d);
    files[count] = NULL; // Since we allocate extra memory at the end of the loop this is guaranteed to work
    return files;
}

int link_to_target(const char* dir, const char* target)
{
    char command[1024];
    int bytes = snprintf(command, sizeof(command), "gcc %s/*.o -o %s", dir, target);
    if (bytes < 14)
    {
        flog(LOG_ERROR, "failed to create link command: %s", strerror(errno));
        flog(LOG_INFO, "print command (might segfault) (y/N)");
        char choice = getchar();
        while (getchar() != '\n');
        if (toupper(choice) != 'Y')
            exit(1);
        
        printf("command: '%s'\n", command);
        flog(LOG_INFO, "continue? (y/N)");
        
        choice = getchar();
        while (getchar() != '\n');
        if (toupper(choice) != 'Y')
            exit(1);

        flog(LOG_INFO, "continuing... (you may have a deathwish)");
        sleep(1);
    }
    flog(LOG_INFO, "/bin/sh: %s", command);
    if (system(command))
    {
        return 1;
    }
    return 0;
    // const char** ls = get_directory_list(dir) ;
    // for (int i = 0; ls[i]; i++)
    // {
    //     const char* command[] = {"ld", ls[i], "-o", target, NULL};
    //     run_command(command);
    // }
}
