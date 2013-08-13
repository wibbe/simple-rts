#!/bin/sh

mkdir -p $1

echo "Copying $2/data to $1"
ln -s $2/data $1

echo "Copying $2/packages to $1"
ln -s $2/packages $1
