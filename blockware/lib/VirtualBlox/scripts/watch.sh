#!/bin/sh

# BLOCKWARE_NAME is required
BLOCKWARE_NAME=$1
if [ -z $BLOCKWARE_NAME ]; then
   echo "❌ Must specify a BLOCKWARE_NAME; e.g. ./watch.sh pixel-dust"
   exit 1
fi

echo "> 📦 Ensuring script dependencies are installed ..."
cd scripts
yarn
cd ..

node ./scripts/watch.js $1




