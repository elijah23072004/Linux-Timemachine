#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include "utils.cpp"
#include "fullBackup.cpp"
#include "differentialBackup.cpp"
#include "backupHandler.h"
#define PROJECT_NAME "linuxTimeMachine"

BackupHandler::BackupHandler(){
    this->configFileLocation = std::string(std::getenv("HOME")) + "/.config/TimeMachine/config.conf";
    parseConfigFile();

}
BackupHandler::BackupHandler(int argc, char** argv){
    this->configFileLocation=std::string(std::getenv("HOME"))+"/.config/TimeMachine/config.conf";
    if(argc ==1){
        parseConfigFile();
        return;
    }
    handleArguments(argc,argv);

}
void BackupHandler::handleArguments(int argc, char** argv){
    if(strcmp(argv[1], "-h") == 0  || strcmp(argv[1], "--help")==0){
        this->outputHelp();
        return;
    }
    if(strcmp(argv[1], "-p") == 0 || strcmp(argv[1], "--print")==0){
        this->printConfigFile();
        return;
    }
    if(strcmp(argv[1], "-c") == 0 ){
        if(argc!=2){
            this->configFileLocation=argv[2];
        }
        this->parseConfigFile();
        return;
    }
    //if argument  1 in parameters is -r then recover files
    else if(strcmp(argv[1], "-r") ==0){
        if(argc==6){
            fs::path relativePath = fs::path(argv[2]);
            this->outputLocation = argv[4];
            this->backupFolderLocation= argv[3];
            //relative path relative to backup folder location and the epoch time {i/f} folder after  
            fs::path backup = argv[5];

            std::cout<<relativePath.string()<<std::endl;
            std::cout<<this->backupFolderLocation.string()<<std::endl;
            std::cout<<this->outputLocation.string()<<std::endl;
            std::vector<fs::path> backupLocations = findBackupRecoveryList(backupFolderLocation, backup);
            std::cout << recoverFilesRecursively(relativePath, backupLocations, this->outputLocation, backupFolderLocation) <<std::endl;
            return;

        }
        else{
            std::cout<<"please pass arguements for location of file to be recovered, backup folder location  and output location for recovered files and backupId "<<std::endl;
            setOutputCode(1);
            return;
        }
    }
    else{
        //otherwise run backup using paramers and flags passed to function
        if(argc <=3){
            this->outputHelp();
            return;
        }
        this->inputLocation = argv[1];
        this->outputLocation = argv[2];
        
        std::string flags = argv[3];
        if(flags[0] != '-'){
            std::cout<<"invalid string second argument"<<std::endl;
            setOutputCode(-1);
            return;
        }
        flags = flags.substr(1);

        for(char val : flags){
            if(val == 'c'){
                if (this->compression){
                    std::cout<<"c in flag argument multiple times"<<std::endl;
                    setOutputCode(-1);
                    return;
                }
                this->compression=true;
            }
            else if(val == 'f' || val == 'd' || val == 'r'){
                if(this->backupMode != '0'){
                    std::cout<<"multiple backup modes in flag argument"<<std::endl;
                }
                this->backupMode =val; 
                if(val == 'r')this->backupMode = std::stoi(argv[4]);
            }
        }
        this->executeBackup();
    }
    return;

}

void BackupHandler::executeBackup(){
    if( this->backupMode == 'f' ){
        std::cout<<fullBackup(this->inputLocation,this->outputLocation)<<std::endl;
    }
    else if (this->backupMode == 'd'){
        std::cout<<differentialBackup(this->inputLocation, this->outputLocation)<<std::endl;
    }
    else if (this->backupMode == 'r'){
        int numberPreviousBackups = findRecentBackups(this->outputLocation).size();
        std::cout<<numberPreviousBackups -1 << " backups done since last full backup" <<std::endl;
        if(numberPreviousBackups == 0 || numberPreviousBackups>=this->backupRatio){
            std::cout<<fullBackup(this->inputLocation,this->outputLocation, this->compression)<<std::endl;
        }
        else{
            std::cout<<differentialBackup(this->inputLocation,this->outputLocation, this->compression)<<std::endl;
        }
    }
    else{
        throw std::invalid_argument(std::string("invalid backup mode :")+this->backupMode);
    }
    return;
}
void BackupHandler::setOutputCode(int code){
    this->outputCode=code;
}
int BackupHandler::getOutputCode(){
    return this->outputCode;
}
void BackupHandler::parseConfigFile(){
    if(!doesPathExist(configFileLocation.string())){
        throw std::invalid_argument(configFileLocation.string() + " does not exist");
    }
    //how many x differential backups till a full backup should be done 
    std::ifstream myFile(configFileLocation);
    std::string text;

    //reads text file in fileLocation line by line reading corresponding parameters from [key] where key is parameter name
    while (getline(myFile, text)){
        text = trimWhitespace(text);
        if(text == "[input]"){
            if(!this->inputLocation.empty()){
                throw std::invalid_argument("Multiple input locations in file");
            }
            getline(myFile,text);
            text=trimWhitespace(text);
            this->inputLocation = text;
        }
        else if(text == "[output]"){
            if(!this->outputLocation.empty()){
                throw std::invalid_argument("Multiple output locations in config file");
            }
            getline(myFile,text);
            text=trimWhitespace(text);
            this->outputLocation=text;
        }
        else if(text == "[backupMode]"){
            if(this->backupMode != '0'){
                throw std::invalid_argument("Multiple backup modes in config file");
            }
            getline(myFile,text);
            text=trimWhitespace(text);
            if(text=="full" || text=="diff"){
                this->backupMode=text[0];
            }
            else{
                throw std::invalid_argument("Invalid backup Mode in config file");
            }
        }
        else if(text =="[backupRatio]"){
            if(this->backupMode != '0'){
                throw std::invalid_argument("Cannot schedule backups with a backup mode set");
            }
            if(this->backupRatio != 0){
                throw std::invalid_argument("cannot have 2 backup ratios in backup file");
            }
            getline(myFile,text);
            text = trimWhitespace(text);
            this->backupRatio = stoi(text);
            this->backupMode = 'r';
            if(this->backupRatio <0){
                throw std::invalid_argument("cannot have ratio between backups as a negative number");
            }
        }
        else if(text =="[compression]"){
            getline(myFile,text);
            text = trimWhitespace(text);
            if(text!="true" && text != "false"){
                throw std::invalid_argument("compression must be either true or false");
            }
            this->compression=(text=="true");
        }
        else if(text =="[includeHiddenFiles]"){
            getline(myFile,text);
            text = trimWhitespace(text);
            if(text!="true" && text != "false"){
                throw std::invalid_argument("include hidden files must be either true or false");
            }
            this->includeHiddenFiles=(text=="true");
        }
    }
    if(this->inputLocation.empty() || this->outputLocation.empty() || this->backupMode == '0') {
        std::cout<<"field empty in "<<std::endl;
        std::cout<<"input location: "<<this->inputLocation.string()<<std::endl;
        std::cout<<"output location: "<<this->outputLocation.string()<<std::endl;
        std::cout<<"backup mode: "<<this->backupMode<<std::endl;
        throw std::invalid_argument("empty field in config file");
    }
    executeBackup();

}

void BackupHandler::outputHelp(){
    std::string text = "TimeMachine a backup program for linux based systems.\n"
        "--help or -h for help\n"
        "-p or --print to print config file\n"
        "example usage \"timeMachineCLI\" - uses config file in config folder\n"
        "other usage \"timeMachineCLI -c <file>\" uses config file in path in <file>\n"
        "other usage \"timeMachineCLI <inputLocation> <outputLocation <flags>\n"
        "to recover file \"timeMachineCLI -r <relativePath> <backupLocation> <outputLocation> <backupName>\"\n"
        "flags:\n"
        "-c config file location only works if no other flags and next arument is path of config file and no other aruments\n"
        "-r recover file see above\n"
        "-d differential backup\n"
        "-f full backup\n"
        "-r auto decide backup type with an extra argument called ratio after which is the number of backups to do where a full backup is done\n"
        "-c in combination with other flags compression for backup";

    std::cout<<text<<std::endl;
}
void BackupHandler::printConfigFile(){
    if(!doesPathExist(configFileLocation.string())){
        throw std::invalid_argument(configFileLocation.string() + " does not exist");
    }
    //how many x differential backups till a full backup should be done 
    std::ifstream myFile(configFileLocation);
    std::string text;   
    std::cout<<"Config file contents from file: " <<configFileLocation<<std::endl;
    while(getline(myFile,text)){
        std::cout<<text<<std::endl;
    }

}
int main(int argc, char **argv) {
    BackupHandler handler = BackupHandler(argc, argv);
    return handler.getOutputCode();
}
