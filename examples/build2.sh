#!/usr/bin/env bash

set -xe

rm -f bbb

c++ ImplicitMessagingExample2.cpp -I../src -L./build/src -l:libEIPScannerS.a -o bbb -g


