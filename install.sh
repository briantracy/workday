#!/bin/bash

# script will stop if an error occurs
# most likely you dont have gcc
set -e

echo "=== workday install script ==="


echo "downloading files from github ..."
curl -o workday.c 'https://raw.githubusercontent.com/briantracy/workday/master/workday.c'
curl -o workday.1 'https://raw.githubusercontent.com/briantracy/workday/master/workday.1'

echo "compiling source file ..."
gcc -Wall -Wextra -Werror -Wpedantic workday.c

echo "moving binary to /usr/local/bin/workday ..."
mv ./a.out /usr/local/bin/workday

echo "moving man page to /usr/share/man/man1/workday.1 ..."
mv ./workday.1 /usr/share/man/man1/workday.1

echo "=== done with workday install script ==="
