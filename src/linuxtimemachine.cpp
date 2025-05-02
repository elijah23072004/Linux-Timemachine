#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include "utils.cpp"
#include "fullBackup.cpp"
#include "differentialBackup.cpp"

#define PROJECT_NAME "linuxTimeMachine"
int handleInput(int,char **);
int parseConfigFile(std::string);
int main(int argc, char **argv) {

    if(argc == 1) {
        std::string defaultLocation = std::string(std::getenv("HOME")) + "/.config/TimeMachine/config.conf";
        return parseConfigFile(defaultLocation);;
    }
    return handleInput(argc,argv);
}

int handleInput(int argc, char ** argv){
    std::string inputLocation;
    std::string outputLocation;
    std::string configLocation;
    //if argument 1 is -c then passed a path of specific config file to use as input
    if(strcmp(argv[1], "-c") == 0 ){
        if(argc==2){
            configLocation = std::string(std::getenv("HOME")) + "/.config/LinuxTimeMachine/config.conf";
        }
        else{
            configLocation=argv[2];
        }
        return parseConfigFile(configLocation);
    }
    //if argument  1 in paramets is -r then recover files
    else if(strcmp(argv[1], "-r") ==0){
        if(argc==6){
            fs::path relativePath = fs::path(argv[2]);
            fs::path outputLocation = argv[4];
            fs::path backupFolderLocation= argv[3];
            //relative path relative to backup folder location and the epoch time {i/f} folder after  
            fs::path backup = argv[5];

            std::cout<<relativePath.string()<<std::endl;
            std::cout<<backupFolderLocation.string()<<std::endl;
            std::cout<<outputLocation.string()<<std::endl;
            std::vector<fs::path> backupLocations = findBackupRecoveryList(backupFolderLocation, backup);
            std::cout << recoverFilesRecursively(relativePath, backupLocations, outputLocation, backupFolderLocation) <<std::endl;
            return 0;
            
        }
        else{
            std::cout<<"please pass arguements for location of file to be recovered, backup folder location  and output location for recovered files and backupId "<<std::endl;
            return 1;
        }
    }
    else{
        //otherwise run backup using paramers and flags passed to function
        inputLocation = argv[1];
        outputLocation = argv[2];
        std::string flags = argv[3];
        bool compression = false;
        char backupMode = '0';
        int backupRatio;
        if(flags[0] != '-'){
            std::cout<<"invalid second argument"<<std::endl;
            return -1;
        }
        flags = flags.substr(1);
        
        for(char val : flags){
            if(val == 'c'){
                if (compression){
                    std::cout<<"c in flag argument multiple times"<<std::endl;
                    return -1;
                }
                compression=true;
            }
            else if(val == 'f'){
                if(backupMode != '0'){
                    std::cout<<"multiple backup modes in flag argument"<<std::endl;
                }
                backupMode ='f'; 
            }
            else if (val == 'd'){
                if(backupMode != '0'){
                    std::cout<<"multiple backup modes in flag argument"<<std::endl;
                }
                backupMode ='d'; 
            }
            else if ( val == 'r'){
                backupRatio = std::stoi(argv[4]);
                if(backupMode != '0'){
                    std::cout<<"multiple backup modes in flag argument"<<std::endl;
                }
                backupMode = 'r';
            }
        }

        if( backupMode == 'f' ){
            
            std::cout<<fullBackup(inputLocation,outputLocation)<<std::endl;
        }
        else if (backupMode == 'd'){
            std::cout<<differentialBackup(inputLocation, outputLocation)<<std::endl;
        }
        else if (backupMode == 'r'){
            
            int numberPreviousBackups = findRecentBackups(outputLocation).size();
            std::cout<<numberPreviousBackups -1 << " backups done since last full backup" <<std::endl;
            if(numberPreviousBackups == 0 || numberPreviousBackups>=backupRatio){
                std::cout<<fullBackup(inputLocation,outputLocation, compression)<<std::endl;
                return 0;
            }
            else{
                std::cout<<differentialBackup(inputLocation,outputLocation, compression)<<std::endl;
                return 0;
            }
            }
            else{
                std::cout<<"invalid backup mode"<<std::endl;
            }
    }
    return 0;
}



int parseConfigFile(std::string fileLocation){
    if(!doesPathExist(fileLocation)){
        throw std::invalid_argument(fileLocation + " does not exist");
    }
    std::string inputLocation="";
    std::string outputLocation="";
    std::string backupMode="";
    bool compression = false;
    bool includeHiddenFiles=false;
    //how many x differential backups till a full backup should be done 
    bool schedule= false;
    int backupRatio = 0;
    std::ifstream myFile(fileLocation);
    std::string text;

    //reads text file in fileLocation line by line reading corresponding parameters from [key] where key is parameter name
    while (getline(myFile, text)){
        text = trimWhitespace(text);
        if(text == "[input]"){
            if(inputLocation != ""){
                throw std::invalid_argument("Multiple input locations in file");
            }
            getline(myFile,text);
            text=trimWhitespace(text);
            inputLocation = text;
        }
        else if(text == "[output]"){
            if(outputLocation != ""){
                throw std::invalid_argument("Multiple output locations in config file");
            }
            getline(myFile,text);
            text=trimWhitespace(text);
            outputLocation=text;
        }
        else if(text == "[backupMode]"){
            if(backupMode != ""){
                throw std::invalid_argument("Multiple backup modes in config file");
            }
            getline(myFile,text);
            text=trimWhitespace(text);
            if(text=="full"){
                backupMode="full";
            }
            else if(text=="diff"){
                backupMode="diff";
            }
            else{
                throw std::invalid_argument("Invalid backup Mode in config file");
            }
        }
        else if(text =="[backupRatio]"){
            if(backupMode != ""){
                throw std::invalid_argument("Cannot schedule backups with a backup mode set");
            }
            if(backupRatio != 0){
                throw std::invalid_argument("cannot have 2 backup ratios in backup file");
            }
            getline(myFile,text);
            text = trimWhitespace(text);
            backupRatio = stoi(text);
            schedule=true;
            if(backupRatio <0){
                throw std::invalid_argument("cannot have ratio between backups as a negative number");
            }
        }
        else if(text =="[compression]"){
            getline(myFile,text);
            text = trimWhitespace(text);
            if(text == "true"){
                compression=true;
            }
            else if(text == "false"){
                compression=false;
            }
            else{
                throw std::invalid_argument("compression must be either true or false");
            }
        }
        else if(text =="[includeHiddenFiles]"){
            getline(myFile,text);
            text = trimWhitespace(text);
            if(text == "true"){
                includeHiddenFiles=true;
            }
            else if(text=="false"){

                includeHiddenFiles=false;    
            }
            else{
                throw std::invalid_argument("include hidden files must be either true of false");
            }
        }
    }
    if(inputLocation.empty() || outputLocation.empty() || (backupMode.empty() && !schedule )) {
        std::cout<<"field empty in "<<std::endl;
        std::cout<<"input location: "<<inputLocation<<std::endl;
        std::cout<<"output location: "<<outputLocation<<std::endl;
        std::cout<<"backup mode: "<<backupMode<<std::endl;
        throw std::invalid_argument("empty field in config file");
    }
    if(schedule){

        int numberPreviousBackups = findRecentBackups(outputLocation).size();
        std::cout<<numberPreviousBackups -1 << " backups done since last full backup" <<std::endl;
        if(numberPreviousBackups == 0 || numberPreviousBackups>=backupRatio){
            std::cout<<fullBackup(inputLocation,outputLocation, compression, includeHiddenFiles)<<std::endl;
            return 0;
        }
        else{
            std::cout<<differentialBackup(inputLocation,outputLocation, compression, includeHiddenFiles)<<std::endl;
            return 0;
        }
    }
    else if(backupMode == "full"){
        std::cout<<fullBackup(inputLocation,outputLocation)<<std::endl;
        return 0;
    }
    else if(backupMode == "diff"){
        std::cout<<differentialBackup(inputLocation, outputLocation)<<std::endl;
        return 0;
    }
    else{
        std::cout<<"invalid backup mode of: "<<backupMode<<std::endl;
        throw std::invalid_argument("Invalid backup Mode");
    }




}
