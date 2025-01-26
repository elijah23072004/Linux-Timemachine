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
    std::ofstream of;
    of.open(backupLogFile.c_str(), std::ios::app);
    of<<backupName;
    of<<"\n";
    of.close();
    
    if (backupName.back() == 'f'){
        of.open(fullBackupLog.c_str());
    }
    else{
        of.open(fullBackupLog.c_str(), std::ios::app);
    }
    of<<backupName;
    of<<"\n";
    of.close();
    
    
}

void createParentFolderIfDoesntExist(fs::path path){
    std::cout<<path.string()<<std::endl;
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
        std::string command = "patch -N " + outputLocation.string() + " < " + (backupFolder/location/relativePath).string();
        std::cout<<command<<std::endl;
        system(command.c_str());
    }
    return outputLocation;

}


void recoverFileTest(){
    std::vector<fs::path> locs;
    locs.push_back("f1");
    locs.push_back("f2");
    locs.push_back("f3");
    locs.push_back("f4");
    locs.push_back(".f5");
    fs::path outputLocation = "./rec/temp/";
    fs::path relativePath = "file.txt";
    std::cout<<(recoverFile(relativePath,locs, outputLocation, ".rec/")).string()<<std::endl;

}


#endif
