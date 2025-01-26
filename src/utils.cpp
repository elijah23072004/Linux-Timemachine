#ifndef UTILS 
#define UTILS

#include <iostream>
#include <string>
#include <filesystem>
#include <time.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>

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

#endif
