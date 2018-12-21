#!/bin/sh

cd /bos-mnt/bos.pegtoken

rm ./bos.pegtoken.abi
eosio-abigen -contract=bos.pegtoken -output=./bos.pegtoken.abi   ./bos.pegtoken.cpp 

rm ./bos.pegtoken.wast
eosio-cpp -o ./bos.pegtoken.wast ./bos.pegtoken.cpp

rm ./bos.pegtoken.wasm
eosio-cpp -o ./bos.pegtoken.wasm ./bos.pegtoken.cpp
