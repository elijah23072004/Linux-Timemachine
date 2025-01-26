#ifndef FULLBACKUP
#define FULLBACKUP


#include <iostream>
#include <string>
#include <filesystem>
#include <time.h>
#include <sys/stat.h>
#include "utils.cpp"

namespace fs = std::filesystem;

//fullbackup takes 2 arguments 
//input path which is the path for everything inside folder path to be backed up 
//outputPath which is the path for everything to be backed up to
//returns 0 for sucess, 1 for out of space error, 2 if either path is not a folder,#
//3 if folder with same epoch time already exist 
//4 if invalid permissions for either inputPath or outputPath 
int fullBackup(fs::path inputPath, fs::path outputPath){
    if(!(isDirectory(inputPath) && isDirectory(outputPath))) return 2;
    long epochTime = std::time(0);
    if(doesEpochBackupExists(outputPath, epochTime)) return 3;

    std::string newFolderName = std::to_string(epochTime) + "f"; 
    outputPath.append(newFolderName);
    std::cout<<outputPath.string()<<std::endl;
    try{ 
        if(!fs::create_directory(outputPath)) return 4;
    }
    catch (const std::filesystem::filesystem_error &ex){
        return 4;
            
    }
    fs::copy_options options = fs::copy_options::recursive;
                    

    fs::copy(inputPath, outputPath, options);

    logBackup(newFolderName, outputPath.parent_path());

    return 0;
}
#endif
