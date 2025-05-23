#ifndef FULLBACKUP
#define FULLBACKUP


#include <iostream>
#include <string>
#include <filesystem>
#include <time.h>
#include <sys/stat.h>
#include "utils.cpp"

namespace fs = std::filesystem;

//fullbackup runs a full backup storing all files from inputPath in outputPath and will compress the backup if compression is true otherwise will not compress backup
//input path which is the path for everything inside folder path to be backed up 
//outputPath which is the path for everything to be backed up to
//returns 0 for sucess, 1 for out of space error
//2 if either path is not a folder
//3 if folder with same epoch time already exist 
//4 if invalid permissions for either inputPath or outputPath 
int fullBackup(fs::path inputPath, fs::path outputPath, bool compression=false, bool includeHiddenFiles=false){
    //if either of inputPath or outputPath is not a directory return 
    if(!(isDirectory(inputPath) && isDirectory(outputPath))) return 2;
    long epochTime = std::time(0);
    //if backup already exists return
    if(doesEpochBackupExists(outputPath, epochTime)) return 3;
    fs::path unchangedOutputPath = outputPath;
    std::string newFolderName = std::to_string(epochTime) + "f"; 
    outputPath.append(newFolderName);
    std::cout<<outputPath.string()<<std::endl;
    try{ 
        if(!fs::create_directory(outputPath)) return 4;
    }
    catch (const std::filesystem::filesystem_error &ex){
        return 4;

    }

    std::vector<fs::path> folderQueue;
    folderQueue.push_back(inputPath);
    fs::path current_path = inputPath;
    while(!folderQueue.empty())
    {
        current_path=folderQueue.at(0);
        folderQueue.erase(folderQueue.begin());
        for (const fs::path & entry : fs::directory_iterator(current_path)){
            //if includeHiddenFiles is false and relative path starts with . then a hidden file and skip
            unsigned int inputPathLength = inputPath.string().size();
            std::string relativePath = entry.string().substr(inputPathLength);

            if(entry == unchangedOutputPath){continue;}

            if(relativePath[0] == '/'){
                relativePath = relativePath.substr(1);
            }
            std::string fileName = entry.filename();
            if(!includeHiddenFiles && fileName[0] == '.'){
                continue;
            }
            if (isDirectory(entry)){
                folderQueue.push_back(entry);
                //add directory to queue to go to next 
            }
            else{
                createParentFolderIfDoesntExist(outputPath/relativePath);
                try{
                    fs::copy(inputPath/relativePath, outputPath/relativePath);
                }
                catch (const std::filesystem::filesystem_error& e){
                    std::cout<<"could not write file: " << (inputPath/relativePath).string() << " to location: " << (outputPath/relativePath).string()<<std::endl;

                }
            }
        }
    }
    std::cout<<"backed up files now compressing files and creating log files"<<std::endl;
    fs::path fileTreeDestination = outputPath.parent_path() / "fileTrees"/newFolderName;
    if(compression){
        long compressionSize = 32768;
        compressBackupDirectory(outputPath, outputPath.parent_path() / "backupMaps"/newFolderName, compressionSize);
    }

    saveFileTree(inputPath, fileTreeDestination,unchangedOutputPath, includeHiddenFiles);
    logBackup(newFolderName, outputPath.parent_path());

    return 0;
}
#endif
