#!/bin/bash

CFLAGS="-Wall -std=c23 -D_DEFAULT_SOURCE -I/usr/include -O3"

TARGET="autoc"
INSTALL_DIR="/usr/local/bin"

xxd -i -n default_config ./src/default_config.ini src/default_config.h

if gcc $CFLAGS ./src/*.c -o $TARGET;
then
    cp $TARGET $INSTALL_DIR/$TARGET
else
    echo "Failed to compile $TARGET, won't install to $INSTALL_DIR"
fi

rm -fr ./bin/*.o    # .o extension in case someone accidentally removes
                    # proceding . and removes /bin if running with sudo
