#!/bin/bash

if [ -z "$1" -o -z "$2" ]; then
    exit 1
fi

if [ -e "$2" ]; then
    echo "File exists"
    exit 1
fi

echo "$2"|cut -d . -f 1 > $2
tail +2 $1 >> $2
