#!/bin/bash
#
# Script to automate library loading
#
# Place this script in /home, chmod +x it to make it executable
# LD_LIBRARY_PATH=/home
# g++ ./malloc_2.cpp -fPIC -shared -o libmal.so
g++ malloc_2.cpp -fPIC -shared -o libmal.so
g++ tamuz_tests_hw4_malloc2.cpp -L ./ -lmal

export LD_LIBRARY_PATH="/home"
./a.out
