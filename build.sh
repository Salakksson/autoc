#!/bin/bash

CFLAGS="-Wall -O3"

TARGET="autoc"

xxd -i -n default_config ./src/default_config.ini src/default_config.h

gcc $CFLAGS ./src/*.c -o $TARGET;
