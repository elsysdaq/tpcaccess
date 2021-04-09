#!/bin/bash
echo $PWD
swig -c++ -python -py3 tpcaccess.i
cp Makefile_python Makefile
make
