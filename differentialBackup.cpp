#ifndef DIFFERENTIAL
#define DIFFERENTIAL
#include <filesystem>
#include <vector>
#include <string>
#include <iostream>
#include "utils.cpp"
#include <sys/stat.h>

namespace fs = std::filesystem;

bool isFileModifiedAfterTime(fs::path file, long time){
    struct stat result;
    stat(file.c_str(), &result);
    std::cout<<result.st_mtime<<std::endl;
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

void changedFileTester(fs::path path){
    //changed in last minute 
    long time = std::time(0) - 60;
    std::vector<fs::path> files =  getAllChangedFiles(path,time);
    for(auto i:files){
        std::cout<<i<<std::endl;
    }

}

#endif
