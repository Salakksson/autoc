# Autoc - Simple Build Tool

#### A build tool designed to make compilation simple

## Features

- **Config file**: Uses an `autoc.ini` file in the project directory to set variables such as compiler flags
- **Smart compilation**: Only compiles files which have changed since they were last compiled

## Compilation

- `install.sh` can be used to compile and install autoc to a specific directory (by default `/usr/local/bin`)
- There are currently no plans to support platforms other than linux by default, it may take some tweaking to work on other platforms such as windows or macos

## Configuration

- `autoc.ini` is a standard ini file which should look like this:

```ini
[general]
src = ./src
bin = ./bin
target = autoc 
ldflags = ; linker flags

[.c]
command = gcc -Wall -c %s -o %o
```

- The `[general]` section controls most of the essencials, it is unnecessary as any unrecognised section will be inferred to be `[general]`
- The `[.extension]` sections control compilation for specific extensions, by default .c files are compiled using `gcc -Wall -c <source> -o <object>`
- The `command` key inserts the source file in `%s` and the object file in `%o`
- If there is no command key present for a file format it will be left untouched

## .ini parser 

- The .ini parser used is inih: `https://github.com/benhoyt/inih`
- In the future a custom parser will be written to support multiline scripts for compiling more complicated projects

## Licencing & contibuting

- If this code results in your computer setting aflame or any other issues you are responsible.
- If you choose to contribute keep the code as simple as possible, don't add unnecessary dependencies.

