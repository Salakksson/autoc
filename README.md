# Autoc - Simple Build Tool

#### A simple build tool designed to make compilation simple

## Features

- **Config file**: The tool uses an `autoc.ini` file in a repo directory to set variables such as compiler flags

## Compilation

- `install.sh` can be used to compile and install autoc to a specific directory

## Configuration

- `autoc.ini` is a standard ini file which should look like this:
```ini
[general]
src = ./src
bin = ./bin
target = autoc 
; ldflags = "-lm -lraylib"

[.c]
; command = "gcc %i -o %o"
```
- The `[general]` section controls most of the essencials, it is unnecessary as any unrecognised section will be inferred to be `[general]`
- The `[.extension]` sections control compilation for specific extensions, by default .c files are compiled using `gcc INPUT -o OUTPUT`

## .ini parser

- The .ini parser used is inih: `https://github.com/benhoyt/inih`
