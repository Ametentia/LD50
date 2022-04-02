#!/bin/sh

if [[ ! -d "../build" ]];
then
    mkdir "../build"
fi

pushd "../build" > /dev/null

COMPILER_FLAGS="-Wall -Wno-unused-function -Wno-switch -I ../base"
LINKER_FLAGS="-ldl -lpthread -lSDL2"

# Build renderer
#
"../base/renderer/linux_glx.sh" debug

# Debug build
#
g++ -O0 -g -ggdb -DLUDAM_INTERNAL=1 $COMPILER_FLAGS "../code/linux_ludum.cpp" -o "ludum.bin" $LINKER_FLAGS

popd > /dev/null
