#ifndef UTILS 
#define UTILS

#include <iostream>
#include <string>
#include <filesystem>
#include <time.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>

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
void writeToFile(fs::path path, std::string text, bool append=false){
    std::ofstream of;
    if(append){
        of.open(path.c_str(), std::ios::app);
    }
    else{
        of.open(path.c_str());
    }
    of<<text;
    of.close();
    return;    
}
//returns true if a folder with epochtime +'i' or +'f' exists 
//takes fs::path path and long epochtime as arguments
bool doesEpochBackupExists(fs::path path, long epochTime){
    std::string dirName = std::to_string(epochTime) + 'f';
    path.append(dirName);

    if(doesPathExist(path)) return true;
    dirName[dirName.size()-1] = 'i';
    path.replace_filename(dirName);
    if(doesPathExist(path)) return true;
    return false;
}

void logBackup(std::string backupName, fs::path outputLocation){
    fs::path backupLogFile = outputLocation / "backups.log";
    fs::path fullBackupLog = outputLocation / "fullBackup.log";
    std::string text = backupName + "\n";
    bool append = backupName.back() != 'f';
    writeToFile(fullBackupLog,text, append);
    writeToFile(backupLogFile,text, true);
}

void createParentFolderIfDoesntExist(fs::path path){
    fs::path parentPath = path.parent_path();
    std::vector<fs::path> foldersToCreate;
    while(!doesPathExist(parentPath)){
        foldersToCreate.push_back(parentPath);
        parentPath= parentPath.parent_path();
    }
    for(fs::path folder : foldersToCreate){
        fs::create_directory(folder);
    }
    return;
}
//takes relativePath as arguemnt which is the relative path of a file to the start of a backup 
//and a vector of paths (backupLocations) which stores all the backups to look at with the first in the vector
//to be the full backup
//returns a path to the recovered File 
//which will be stored  in the output location passed in the outputLoc fs::path
fs::path recoverFile(fs::path relativePath, std::vector<fs::path> backupLocations, fs::path outputLocation, fs::path backupFolder){
    if(is_directory(backupLocations.at(0)/relativePath)){
        throw std::invalid_argument("recieved folder as a file to recover");
    }
    fs::path fileLocation = backupFolder / backupLocations.at(0) / relativePath;
    outputLocation/=std::to_string(std::time(0));
    createParentFolderIfDoesntExist(outputLocation);
    fs::copy(fileLocation, outputLocation, fs::copy_options::overwrite_existing);
    backupLocations.erase(backupLocations.begin());
    for(fs::path location : backupLocations){
        std::string command = "patch -Ns " + outputLocation.string() + " < " + (backupFolder/location/relativePath).string();
        //std::cout<<command<<std::endl;
        system(command.c_str());
    }
    return outputLocation;

}

std::vector<fs::path> findBackups(fs::path path){
    fs::path logPath = path/"fullBackup.log";
    std::vector<fs::path> backups;
    std::ifstream f;
    f.open(logPath.c_str());
    std::string line;
    while(std::getline(f,line)){
        if(line == ""){continue;}
        backups.push_back(fs::path{line});
    }
    return backups;
    
}

std::string getFileTree(fs::path path){
    std::string fileTree = "";
    std::vector<fs::path> folderQueue;
    folderQueue.push_back(path);
    fs::path currentPath;
    while(!folderQueue.empty()){
        currentPath = folderQueue.at(0);
        folderQueue.erase(folderQueue.begin());
        for(const  fs::path & entry : fs::directory_iterator(currentPath)){
            fileTree += entry.string() + "\n";
            if(is_directory(entry)){
                folderQueue.push_back(entry);
            }
        }
    }
    //remove \n at end of string only if string is not empty 
    if(fileTree != ""){
        return fileTree.substr(0,fileTree.size()-1);
    }
    return fileTree;
}
//takes inputPAth as argumetn which is location where file tree should be made of 
//and destination as arguement which is the full path of where to save file tree
void saveFileTree(fs::path inputPath, fs::path destination){
    //creates folder to store destination if doesn't exist
    std::string tree = getFileTree(inputPath);
    createParentFolderIfDoesntExist(destination);
    writeToFile(destination, tree);
}

#endif

