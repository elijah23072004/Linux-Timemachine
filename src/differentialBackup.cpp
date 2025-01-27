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

bool isFileModifiedAfterTime(fs::path file, long time){
    struct stat result;
    stat(file.c_str(), &result);
    return result.st_mtime >= time;
    
}

std::vector<fs::path> getAllChangedFiles(fs::path path, long prevTime){

    std::vector<fs::path> changedFiles;
    std::vector<fs::path> folderQueue;
    folderQueue.push_back(path);
    fs::path current_path = path;
    while(!folderQueue.empty())
    {
        current_path=folderQueue.at(0);
        folderQueue.erase(folderQueue.begin());
        for (const auto & entry : fs::directory_iterator(current_path)){
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




int calcualteAndStoreDiffFile(fs::path inputFile, fs::path oldFile, fs::path destinationLocation){
    // -N makes sure that absent files are counted as empty
    std::string command = "diff " + oldFile.string() + " " +  inputFile.string() 
        + " -N "  +  " > " + destinationLocation.string();
    //std::cout<<command<<std::endl;
    return system(command.data());
}


//differentialBackup takes 2 arguments 
//currently just finds last full backup and compares to it
//input path which is the path for everything inside folder path to be backed up 
//outputPath which is the path for everything to be backed up to
//returns 0 for sucess, 1 for out of space error, 2 if either path is not a folder,#
//3 if folder with same epoch time already exist 
//4 if invalid permissions for either inputPath or outputPath 
//5 if no previous fullBackup exists
int differentialBackup(fs::path inputPath, fs::path outputPath){

    if(!(isDirectory(inputPath) && isDirectory(outputPath))) return 2;
    long epochTime = std::time(0);
    if(doesEpochBackupExists(outputPath, epochTime)) return 3;

    std::vector<fs::path> backups = findBackups(outputPath);
    long lastBackupTime= std::stoll(backups.at(backups.size()-1).string().substr(0, backups.at(0).string().size()-1));
    
   
    if(lastBackupTime ==0){return 5;}


    std::cout<<"time of last backup: "<<lastBackupTime<<std::endl;

    std::string newFolderName = std::to_string(epochTime) + "i"; 
    fs::path outputFolder = outputPath;
    outputPath.append(newFolderName);
    std::cout<<"output location: "<<outputPath.string()<<std::endl;
    try{ 
        if(!fs::create_directory(outputPath)) return 4;
    }
    catch (const std::filesystem::filesystem_error &ex){
        return 4;
            
    }
    
    std::vector<fs::path> changedFiles = getAllChangedFiles(inputPath, lastBackupTime);
    
    //temparily just store full files (incremental backup) 
    int inputPathLength = inputPath.string().size();
    std::cout<<"number of changed files: "<<changedFiles.size()<<std::endl;
    if(changedFiles.size() == 0){
        return 0;
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
    saveFileTree(inputPath, fileTreeDestination);
    logBackup(newFolderName, outputFolder);
    return 0;
}


void changedFileTester(fs::path path){
    //changed in last minute 
    long time = std::time(0) - 60;
    std::vector<fs::path> files =  getAllChangedFiles(path,time);
    for(auto i:files){
        std::cout<<i<<std::endl;
    }

}

#endif
