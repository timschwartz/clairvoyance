#!/bin/bash

NOW=`date +"%Y-%m-%d %H:%M:%S"`
HOST=`hostname`

echo "#define BUILD_VERSION \"$HOST|$NOW\"" > ../include/build.h
