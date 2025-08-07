#!/bin/bash
build_dir="./build_dir"
meson setup $build_dir

if meson compile -C $build_dir; then

    cd $build_dir 
    meson test
    cd ..
    cat ${build_dir}/meson-logs/testlog.txt
else 
    echo "Compilation failed"
fi


