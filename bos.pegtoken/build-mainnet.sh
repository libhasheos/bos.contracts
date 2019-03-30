#!/bin/sh

YAML=docker-compose.yml
CONTAINER=cdt
MNT_PATH=/bos-mnt
CONTRACT_NAME=bos.pegtoken

docker-compose -f $YAML up -d

docker-compose -f $YAML exec $CONTAINER bash -c "rm -rf $MNT_PATH/$CONTRACT_NAME.abi $MNT_PATH/$CONTRACT_NAME.wast $MNT_PATH/$CONTRACT_NAME.wasm"
docker-compose -f $YAML exec $CONTAINER bash -c "cd $MNT_PATH && eosio-cpp -abigen $MNT_PATH/src/$CONTRACT_NAME.cpp --contract=$CONTRACT_NAME -abigen_output=$MNT_PATH/$CONTRACT_NAME.abi -I=$MNT_PATH/include/"
docker-compose -f $YAML exec $CONTAINER bash -c "cd $MNT_PATH && eosio-cpp -I=$MNT_PATH/include/ -o $MNT_PATH/$CONTRACT_NAME.wast $MNT_PATH/src/$CONTRACT_NAME.cpp"
docker-compose -f $YAML exec $CONTAINER bash -c "cd $MNT_PATH && eosio-cpp -I=$MNT_PATH/include/ -o $MNT_PATH/$CONTRACT_NAME.wasm $MNT_PATH/src/$CONTRACT_NAME.cpp"

docker-compose -f $YAML down

