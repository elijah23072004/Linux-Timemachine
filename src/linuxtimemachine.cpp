#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include "utils.cpp"
#include "fullBackup.cpp"
#include "differentialBackup.cpp"

#define PROJECT_NAME "linuxTimeMachine"
int handleInput(int,char **);
int parseConfigFile(std::string);
int main(int argc, char **argv) {

    if(argc == 1) {
        return parseConfigFile("./config.conf");;
    }
    return handleInput(argc,argv);

    std::cout << "This is project " << PROJECT_NAME << ".\n";
    std::string backupMode = argv[3]; 

    

    return 0;
}

int handleInput(int argc, char ** argv){
    std::string inputLocation;
    std::string outputLocation;
    std::string configLocation;
    if(strcmp(argv[1], "-c") == 0 ){
        if(argc==2){
            configLocation = "./config.conf";
        }
        else{
            configLocation=argv[2];
        }
        return parseConfigFile(configLocation);
    }
    else{
        inputLocation = argv[1];
        outputLocation = argv[2];
        if( strcmp(argv[3], "-f") == 0 ){
            
            std::cout<<fullBackup(inputLocation,outputLocation)<<std::endl;
        }
        else if (strcmp(argv[3] ,"-d") == 0){
            std::cout<<differentialBackup(inputLocation, outputLocation)<<std::endl;
        }
        else{
            std::cout<<"invalid backup mode"<<std::endl;
        }
    }
    return 0;
}

std::string trimWhitespace(std::string str){
    int start=0;
    int end=str.size()-1;
    char whiteSpace[] = {' ', '\t', '\n'};
    for(int i=0;str[i];i++){
        if(strchr(whiteSpace, str[i]) == NULL){
            start=i; 
            break;
        }
    }
    for(int i=str.size(); i>=0;i--){
        if(strchr(whiteSpace, str[i]) == NULL){
            end=i+1; 
            break;
        }
    }

    return str.substr(start,end-start);
}

int parseConfigFile(std::string fileLocation){
    std::string inputLocation="";
    std::string outputLocation="";
    std::string backupMode="";
    bool schedule;
    //how many x differential backups till a full backup should be done 
    int backupRatio = 0;
    int frequency = 0;
    std::ifstream myFile(fileLocation);
    std::string text;
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
        else if(text == "[schedule]"){
            if(backupMode != ""){
                throw std::invalid_argument("Cannot schedule backups with a backup mode set");
            }
            if(schedule){
                throw std::invalid_argument("cannot schedule twice in backup file");
            }
            schedule=true;
            getline(myFile,text);
            text = trimWhitespace(text);
            frequency= stoi(text);
            if(frequency <=0){
                throw std::invalid_argument("cannot have time between backups at zero or below");
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
            if(backupRatio <0){
                throw std::invalid_argument("cannot have ratio between backups as a negative number");
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
        //need to set systemd timer to be the frequency 
        //will need to be checked to see if timer is already set and right frequency

        int numberPreviousBackups = findBackups(outputLocation).size();
        std::cout<<numberPreviousBackups<<std::endl;
        if(numberPreviousBackups == 0 || numberPreviousBackups>=backupRatio){
            std::cout<<fullBackup(inputLocation,outputLocation)<<std::endl;
            return 0;
        }
        else{
            std::cout<<differentialBackup(inputLocation,outputLocation)<<std::endl;
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
