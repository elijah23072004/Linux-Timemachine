#!/bin/bash

build_dir="./build_dir"

rm -r $build_dir

path_loc="/usr/local/bin/"

meson setup $build_dir 
meson compile -C $build_dir 


if ! test -w $path_loc; then 
    echo "Current process does not have permission to write to location $path_loc please run with sufficient permissions (e.g via sudo)"
    rm -r $build_dir 
    exit 1

fi 

cp ${build_dir}/linuxTimeMachineBackend ${path_loc}linuxTimeMachineCLI
cp ${build_dir}/linuxTimeMachineApp ${path_loc}linuxTimeMachine




rm -r $build_dir 
