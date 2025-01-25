#ifndef UTILS 
#define UTILS

#include <iostream>
#include <string>
#include <filesystem>
#include <time.h>
#include <sys/stat.h>

namespace fs = std::filesystem;
//takes path as arguement and returns true if it exists in filesystem
bool doesPathExist(fs::path path){
    return  fs::exists(path.string()); 
}

//returns true if path passed is a directory, and false if it is not a directory
bool isDirectory(fs::path path)
{
    return fs::is_directory(path);
}
//returns true if a folder with epochtime +'i' or +'f' exists 
//takes fs::path path and long epochtime as arguments
bool doesEpochBackupExists(fs::path path, long epochTime){
    std::string dirName = std::to_string(epochTime) + 'f';
    path.append(dirName);

    if(doesPathExist(path)) return true;
    dirName[dirName.size()-1] = 'i';
    std::cout<<dirName<<std::endl;
    path.replace_filename(dirName);
    if(doesPathExist(path)) return true;
    return false;
}

#endif
