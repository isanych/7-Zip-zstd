#!/bin/bash

cd $(dirname $BASH_SOURCE)/CPP/7zip/Bundles/Alone2
make -j -f ../../cmpl_gcc_x64.mak MY_ASM=uasm
