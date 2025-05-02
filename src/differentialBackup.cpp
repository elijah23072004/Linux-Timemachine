#ifndef DIFFERENTIAL
#define DIFFERENTIAL
#include <filesystem>
#include <vector>
#include <string>
#include <iostream>
#include "utils.cpp"
#include <sys/stat.h>
#include <cstdlib>
#include <fstream>
namespace fs = std::filesystem;
//returns true if modification data of file is after time where time is epochTime, otherwise returns false
bool isFileModifiedAfterTime(fs::path file, long time){
    struct stat result;
    stat(file.c_str(), &result);
    return result.st_mtime >= time;
    
}
//iterates over every file inside directory path and will return a vector storing every file in directory 
//where the modification date is after prevTime
//will not include outputPath in return vector 
std::vector<fs::path> getAllChangedFiles(fs::path path, long prevTime, fs::path outputPath, bool includeHiddenFiles=false){

    std::vector<fs::path> changedFiles;
    std::vector<fs::path> folderQueue;
    folderQueue.push_back(path);
    fs::path current_path = path;
    while(!folderQueue.empty())
    {
        current_path=folderQueue.at(0);
        folderQueue.erase(folderQueue.begin());
        for (const fs::path & entry : fs::directory_iterator(current_path)){
            //if includeHiddenFiles is false and relative path starts with . then a hidden file and skip
            if(entry == outputPath){continue;} 
            std::string fileName = entry.filename();
            if(!includeHiddenFiles && fileName[0] == '.'){
                continue;
            }
            if(isFileModifiedAfterTime(entry,prevTime)){
                changedFiles.push_back(entry);
            }
            if (isDirectory(entry)){
                folderQueue.push_back(entry);
                //add directory to queue to go to next 
            }
        }
    }
    return changedFiles;
}

//calculates the diff of inputFile from oldFile and stores output in destinationLocation
int calcualteAndStoreDiffFile(fs::path inputFile, fs::path oldFile, fs::path destinationLocation){
    // -N makes sure that absent files are counted as empty
    std::string command = "diff " + oldFile.string() + " " +  inputFile.string() 
        + " -N "  +  " > " + destinationLocation.string();
    return system(command.data());
}


//Differential backup will run a differential backup in outputPath, storing file deltas of changed files and outputPath with 
//backup name being epoch time of current backup
//input path which is the path for everything inside folder path to be backed up 
//outputPath which is the path for everything to be backed up to
//returns 0 for sucess, 1 for out of space error, 2 if either path is not a folder,#
//3 if folder with same epoch time already exist 
//4 if invalid permissions for either inputPath or outputPath 
//5 if no previous fullBackup exists
int differentialBackup(fs::path inputPath, fs::path outputPath, bool compression=false, bool includeHiddenFiles=false){
    if(!(isDirectory(inputPath) && isDirectory(outputPath))) return 2;
    long epochTime = std::time(0);
    //if backup already exists return
    if(doesEpochBackupExists(outputPath, epochTime)) return 3;

    std::vector<fs::path> backups = findRecentBackups(outputPath);
    long lastBackupTime= std::stoll(backups.at(backups.size()-1).string().substr(0, backups.at(0).string().size()-1));
    
    //if no previous backups return (as full backup will be needed to be executed first) 
    if(lastBackupTime ==0){return 5;}


    std::cout<<"time of last backup: "<<lastBackupTime<<std::endl;

    std::string newFolderName = std::to_string(epochTime) + "i"; 
    fs::path outputFolder = outputPath;
    outputPath.append(newFolderName);
    std::cout<<"output location: "<<outputPath.string()<<std::endl;
    
    std::vector<fs::path> changedFiles = getAllChangedFiles(inputPath, lastBackupTime, outputFolder, includeHiddenFiles);
    
    int inputPathLength = inputPath.string().size();
    std::cout<<"number of changed files: "<<changedFiles.size()<<std::endl;
    //if no changed files then return and do not create backup
    if(changedFiles.size() == 0){
        return 0;
    }
    try{ 
        if(!fs::create_directory(outputPath)) return 4;
    }
    catch (const std::filesystem::filesystem_error &ex){
        //if file system error (like insufficient write permissions return)
        return 4;

    }
    for(fs::path file : changedFiles){
        std::string relativePath = file.string().substr(inputPathLength);
        if(relativePath[0] == '/'){relativePath=relativePath.substr(1);}

        //not recursive by default when no copy options 
        
        //check parent folder exists since if parent folder doesnt exist file will not be created
        createParentFolderIfDoesntExist(outputPath/relativePath);
 
        if(isDirectory(inputPath/relativePath)){
            fs::create_directory(outputPath/relativePath);
        }
        else{

            //recover file from previous diffs to allow diff to be calcualetd from previous differential backups 
            fs::path lastBackupFile = recoverFile(relativePath, backups, outputFolder / "tmp", outputFolder ); 
            calcualteAndStoreDiffFile(inputPath/relativePath, lastBackupFile, outputPath/relativePath);
            
            //delete temporary file storing changed files
            fs::remove(lastBackupFile);

        }
    }
    fs::path fileTreeDestination = outputFolder / "fileTrees" / newFolderName;

    if(compression){
        //make archives of 32kB
        long compressionSize = 32768;
        compressBackupDirectory(outputPath, outputPath.parent_path() / "backupMaps"/newFolderName, compressionSize);
    }
    //create log files and save file tree to backup folder
    saveFileTree(inputPath, fileTreeDestination, outputFolder, includeHiddenFiles);
    logBackup(newFolderName, outputFolder);
    return 0;
}



#endif
