#include "compile.h"

#include <sys/wait.h>
#include <assert.h>
#include <dirent.h>
#include <libgen.h>

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

const char* get_extension(const char* file)
{
    const char* orig = file;
    while(*file) file++;
    while(*file != '.' && file > orig) file--;
    return file > orig ? file : "";
}

char* get_binary_from_source(const char* src, const char* bin_dir)
{
    char* name_buffer = strdup(src);
    char* name = basename(name_buffer);

    size_t output_len = strlen(src) + strlen(bin_dir) + 10;
    char* output = malloc(output_len);
    snprintf(output, output_len, "%s/%s.o", bin_dir, name);

    free(name_buffer);

    return output;
}

char* format_string(const char* fmt, const char* source, const char* output)
{
    int capacity = 20;
    char* buffer = malloc(capacity);

    if (!buffer)
    { flog(LOG_ERROR, "insufficient memory for malloc/realloc"); exit(1);}

    int i = 0;
    enum
    {
        APPEND_FMT,
        CHECK_ARG,
    } state = APPEND_FMT;

    size_t source_len = strlen(source);
    size_t output_len = strlen(output);

    const char* s = fmt;
    while (*s)
    {
        if (i >= capacity)
            buffer = realloc(buffer, capacity *= 2);

        if (!buffer)
        { flog(LOG_ERROR, "insufficient memory for malloc/realloc"); exit(1);}

        switch(state)
        {
        case APPEND_FMT:
            if (*s != '%')
            {
                buffer[i++] = *(s++);
                break;
            }
            state = CHECK_ARG;
            s++;
            break;
        case CHECK_ARG:
            switch(*(s++))
            {
            case '%':
                buffer[i++] = '%';
                state = APPEND_FMT;
                break;
            case 's':
                if (i + source_len >= capacity)
                    buffer = realloc(buffer, capacity *= 2);
                if (!buffer)
                { flog(LOG_ERROR, "insufficient memory for malloc/realloc"); exit(1);}

                strcpy(buffer + i, source);
                i += source_len;
                break;
            case 'o':
                if (i + output_len >= capacity)
                    buffer = realloc(buffer, capacity *= 2);
                if (!buffer)
                { flog(LOG_ERROR, "insufficient memory for malloc/realloc"); exit(1);}

                strcpy(buffer + i, output);
                i += output_len;
                break;
            }
            state = APPEND_FMT;
            break;
        default:
            UNIMPLEMENTED("fuck you");
            break;
        }
    }
    // buffer[i] = 0;
    return buffer;
}

char* get_command(struct config* conf, const char* source, const char* bin_dir)
{
    const char* ext = get_extension(source);
    const char* adequate_command = hget(&conf->commands, ext);
    if (!adequate_command)
    {
        // flog(LOG_INFO, "skipping '%s'", source);
        return NULL;
    }

    char* bin_path = get_binary_from_source(source, bin_dir);

    char* command = format_string(adequate_command, source, bin_path);

    free(bin_path);

    return command;
}

int compile(struct config* conf, const char* source, const char* bin_dir)
{
    if(!*bin_dir)
    {
        flog(LOG_ERROR, "No binary directory set");
        exit(1);
    }
    char* command = get_command(conf, source, bin_dir);
    if (!command) return 0;

    flog(LOG_INFO, "/bin/sh: %s", command);
    if (system(command))
    {
        free(command);
        return 1;
    }
    conf->link_required = true;

    free(command);
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

int link_to_target(struct config* conf)
{
    if (conf->link_command == NULL)
    {
        char command[1024];
        if (!conf->ldflags) conf->ldflags = "";
        int bytes = snprintf(command, sizeof(command), "g++ %s %s/*.o -o %s", conf->ldflags, conf->bin_dir, conf->target);
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
    }
    else
    {
        // Calculate the size of the final command
        size_t size = 0;
        size_t link_command_size = strlen(conf->link_command);
        for (size_t i = 0; i < link_command_size;)
        {
            // TODO(nic): allow for escaped %, like %% or \%
            // so that users can have '%' character in the command
            if (conf->link_command[i] == '%')
            {
                if (i >= link_command_size - 1)
                {
                    flog(LOG_WARNING, "Stray '%%' at end of linker command");
                    i += 1;
                    continue;
                }
                char special_ch = conf->link_command[i + 1];
                switch (special_ch)
                {
                    case 'l':
                        size += strlen(conf->ldflags);
                        break;
                    case 't':
                        size += strlen(conf->target);
                        break;
                    case 'o':
                        size += snprintf(NULL, 0, "%s/*.o", conf->bin_dir);
                        break;
                    default:
                        flog(LOG_WARNING, "Unrecognized '%%%c' special charater in linker command", special_ch);
                }
                i += 2;
            }
            else
            {
                size += 1;
                i += 1;
            }
        }

        // Build command
        char *final_command = malloc(size + 1);
        final_command[size] = '\0';
        size_t count = 0;
        for (size_t i = 0; i < link_command_size;)
        {
            if (conf->link_command[i] == '%')
            {
                if (i >= link_command_size - 1)
                {
                    i += 1;
                    continue;
                }
                char special_ch = conf->link_command[i + 1];
                switch (special_ch)
                {
                    case 'l':
                        memcpy(&final_command[count], conf->ldflags, strlen(conf->ldflags));
                        count += strlen(conf->ldflags);
                        break;
                    case 't':
                        memcpy(&final_command[count], conf->target, strlen(conf->target));
                        count += strlen(conf->target);
                        break;
                    case 'o':
                        count += sprintf(&final_command[count], "%s/*.o", conf->bin_dir);
                        break;
                }
                i += 2;
            }
            else
            {
                final_command[count++] = conf->link_command[i];
                i += 1;
            }
        }

        flog(LOG_INFO, "/bin/sh: %s", final_command);
        if (system(final_command))
        {
            return 1;
        }
    }

    return 0;
}
