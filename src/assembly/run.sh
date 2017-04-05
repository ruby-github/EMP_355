#!/bin/bash

curdir=`pwd`

export LD_LIBRARY_PATH=$curdir/lib
echo "SET LD_LIBRARY_PATH: $LD_LIBRARY_PATH"

./09001Color
