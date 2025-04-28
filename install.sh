#!/bin/bash

build_dir="./build_dir"

rm -r $build_dir

path_loc="/usr/local/bin/"
data_loc="/usr/lib/"

meson setup $build_dir 
meson compile -C $build_dir 


if ! test -w $path_loc; then 
    rm -r $build_dir 
    exit 1

fi 

cp ${build_dir}/linuxTimeMachineBackend ${path_loc}timeMachineCLI
cp ${build_dir}/linuxTimeMachineApp ${path_loc}timeMachine

echo $PATH



mkdir -p ${data_loc}/TimeMachine/
cp ./data/*ui ${data_loc}/TimeMachine/


rm -r $build_dir 
