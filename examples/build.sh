#!/usr/bin/env bash

set -xe

rm -f aaa

cd ./build/

make

cd ..

c++ ImplicitMessagingExample.cpp -I../src -L./build/src -l:libEIPScannerS.a -o aaa -g


