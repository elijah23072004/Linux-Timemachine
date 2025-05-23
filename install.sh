#!/bin/bash

build_dir="./build_dir"

rm -r $build_dir

path_loc="/usr/local/bin/"
data_loc="/usr/lib/"
application_loc="/usr/share/applications/"

meson setup $build_dir 
if meson compile -C ${build_dir}; then


    if ! test -w $path_loc; then 
        rm -r $build_dir 
        exit 1

    fi 

    cp ${build_dir}/linuxTimeMachineBackend ${path_loc}timeMachineCLI
    cp ${build_dir}/linuxTimeMachineApp ${path_loc}timeMachine
    cp ./data/timeMachine.desktop ${application_loc}
    echo $PATH



    mkdir -p ${data_loc}/TimeMachine/
    cp ./data/*ui ${data_loc}/TimeMachine/

else 
    echo "Compilation failed"
fi 

rm -r $build_dir 
