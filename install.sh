#!/bin/bash

rm -r $build_dir

build_dir="./build_dir"
path_loc="/usr/local/bin/"

meson setup $build_dir 
meson compile -C $build_dir 


if test -w $path_loc; then 
    cp ${build_dir}/linuxTimeMachineBackend ${path_loc}linuxTimeMachineCLI
    cp ${build_dir}/linuxTimeMachineApp ${path_loc}linuxTimeMachine
else
    echo "Current process does not have permission to write to location $path_loc please run with sufficient permissions (e.g via sudo)"
fi 




rm -r $build_dir 

