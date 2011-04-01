#!/bin/bash

set $(cat version.txt)
DIR=$1
VER=$2

cd ..
mv $DIR $DIR-$VER
tar czvf $DIR-$VER.tar.gz $DIR-$VER
mv $DIR-$VER $DIR


