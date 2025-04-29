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
#include <cstring>
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

std::string readFromFile(fs::path file){
    std::ifstream myFile(file);
    std::string data = "";
    std::string line; 
    while (getline(myFile, line)){
        data+= line + "\n";
    }
    data=data.substr(0,data.size()-1); //remove trailing \n
    return data;

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
    for(int i=foldersToCreate.size()-1; i>=0; i--){
        fs::create_directory(foldersToCreate.at(i));
    }
    return;
}


std::vector<std::string> split(std::string str, char delimiter)
{
  // Using str in a string stream
    std::stringstream ss(str);
    std::vector<std::string> res;
    std::string token;
    while (getline(ss, token, delimiter)) {
        res.push_back(token);
    }
    return res;
}

//takes relativePath as arguemnt which is the relative path of a file to the start of a backup 
//and a vector of paths (backupLocations) which stores all the backups to look at with the first in the vector
//to be the full backup
//returns a path to the recovered File 
//which will be stored  in the output location passed in the outputLoc fs::path
fs::path recoverFile(fs::path relativePath, std::vector<fs::path> backupLocations, fs::path outputLocation, fs::path backupFolder, fs::path* filename = NULL){
    if(is_directory(backupLocations.at(0)/relativePath)){
        throw std::invalid_argument("recieved folder as a file to recover");
    }
    fs::path fileLocation = backupFolder / backupLocations.at(0) / relativePath;
    if(filename){
        outputLocation/=*filename;
    }
    else{
        outputLocation/=std::to_string(std::time(0));
    }
    createParentFolderIfDoesntExist(outputLocation);
    //std::cout<<backupFolder << " backupMaps " <<backupLocations.at(0).filename()<<std::endl; 
    if(doesPathExist(backupFolder / "backupMaps" / backupLocations.at(0).filename())){
        std::ifstream f;
        f.open( (backupFolder / "backupMaps" / backupLocations.at(0).filename()).c_str() );
        std::string line; 
        bool found = false;
        while(std::getline(f,line)){
            if(found){
                break;
            }
            line = trimWhitespace(line);
            if(line == relativePath.parent_path()){
                std::getline(f,line);
                line = trimWhitespace(line);
                std::vector<std::string> files = split(line, ',');
                for(unsigned int i=0; i<files.size();i++){
                    if(files.at(i) == relativePath){
                        //archive has this file
                        //will override file if file with same name exists in directory however should always call this into tmp directory and shouldn't happen since file names 
                        //will have random numeric number
                        std::string command = "tar -zxvf " + (backupFolder / backupLocations.at(0) /  files.at(0)).string() 
                            + " --directory="+outputLocation.parent_path().string() + " " + relativePath.filename().string() + " >/dev/null";
                        system(command.c_str());
                        fs::rename(outputLocation.parent_path() / relativePath.filename(), outputLocation);
                        found=true;
                        break;
                    }
                }
            }
        }
    }

    else {
        fs::copy(fileLocation, outputLocation, fs::copy_options::overwrite_existing);
    
    }   
    backupLocations.erase(backupLocations.begin());
    for(fs::path location : backupLocations){
        std::string diffLocation = (backupFolder/location/relativePath).string();
        
        //std::cout<<backupFolder / "backupMaps"/ location.filename()<<std::endl;
        if(doesPathExist(backupFolder / "backupMaps" / location.filename()))
        {
            std::ifstream f;
            f.open( (backupFolder / "backupMaps" / location.filename()).c_str() );
            std::string line; 
            bool found = false;
            while(std::getline(f,line)){
                if(found){
                    break;
                }
                line = trimWhitespace(line);
                if(line == relativePath.parent_path().string()){
                    std::getline(f,line);
                    line = trimWhitespace(line);
                    std::vector<std::string> files = split(line, ',');
                    for(unsigned int i=1; i<files.size();i++){
                        if(files.at(i) == relativePath){
                            //archive has this file
                            //will override file if file with same name exists in directory which will happen if multiple different diff files 
                            //so fix is to put it in a tar folder 
                            fs::create_directory(outputLocation.parent_path()/"tarFolder");
                            std::string command = "tar -zxvf " + (backupFolder / location /  files.at(0)).string() 
                                + " --directory="+(outputLocation.parent_path()/"tarFolder").string() + " " + relativePath.filename().string() + " >/dev/null";
                            system(command.c_str());
                            //gets put in relative path like 123/abc where currently just want /abc here 
                            //fix could be to put into compression just the abc part not the other sections
                            

                            diffLocation = outputLocation.parent_path()/"tarFolder" / relativePath.filename();
                        
                            found=true;
                            break;
                        }
                    }
                }
            }
            if(!found){
                //if not found then file was not changed in this backup
                //std::cout<<"file not found in backup : "<<location<<std::endl;
                continue;
            }
        }
        else{
            if(!doesPathExist(diffLocation)){
                continue;
            }
        }
        std::string command = "patch -Ns " + outputLocation.string() + " < " + diffLocation + " >/dev/null";
        //std::cout<<command<<std::endl;
        system(command.c_str());
        fs::remove_all(outputLocation.parent_path()/"tarFolder");
    }
    return outputLocation;

}
//recover file in relativePath with all files inside directories recusively
fs::path recoverFilesRecursively(fs::path relativePath, std::vector<fs::path> backupLocations, fs::path outputLocation, fs::path backupFolder){
    std::cout<<"recover files recursively" <<std::endl;
    //want to find all files in most recent backup to know which files to recover 
    std::string fileTree = readFromFile(backupFolder/ "fileTrees" / backupLocations.at(backupLocations.size()-1));
    std::vector<std::string> files = split(fileTree, '\n');
    std::string basePath = files.at(0);
    files.erase(files.begin());
    std::vector<std::string> filesToRecover;
    std::cout<<relativePath.string()<<std::endl;
    for(std::string file: files){
        //std::cout<<file<<std::endl;
        file = file.substr(basePath.size());
        if(file.at(0) == '/' ){
            file = file.substr(1);
        }
        //std::cout<<file<<std::endl;
        std::string::size_type index = file.find(relativePath.string(), 0);
        //std::cout<<index<<std::endl;
        if(index != 0){
            continue;
        }
        filesToRecover.push_back(file);
    }
    for(std::string file: filesToRecover){
        //finds path relative to the relativePath path, e.g. Documents/work/2.pdf and Documents/  outputs work/2.pdf 
        std::cout<<file<<std::endl;
        std::string relativeFilePath = file.substr(relativePath.parent_path().string().size());
        if(relativeFilePath.at(0) == '/'){
            relativeFilePath=relativeFilePath.substr(1);
        }
        std::cout<<relativeFilePath<<std::endl;
        fs::path filePath = relativeFilePath;
        fs::path fileName = filePath.filename();
        recoverFile(file, backupLocations, outputLocation/filePath.parent_path(), backupFolder, &fileName);
    }

    return outputLocation;
}


std::vector<fs::path> findBackups(fs::path path){
    fs::path logPath = path/"backups.log";
    std::vector<fs::path> backups;
    if(!doesPathExist(logPath)){
        throw std::invalid_argument("file: " + logPath.string() + " does not exist");
        //if file is non existent return empty vec since no backups will have commenced 
        //return backups;
    }
    std::ifstream f;
    f.open(logPath.c_str());
    std::string line;
    while(std::getline(f,line)){
        if(line == ""){continue;}
        backups.push_back(fs::path{line});
    }
    return backups;
    
}
//lastBackup is just the filename of the backup so just {epochTime}{f/i}
std::vector<fs::path> findBackupRecoveryList(fs::path logLocation, fs::path lastBackup){
    std::vector<fs::path> backups = findBackups(logLocation);
    int lastFullBackup = 0;
    int end;
    bool found=false;
    for(unsigned int i=0; i<backups.size(); i++){
        if(backups.at(i).string().back() == 'f'){
            lastFullBackup = i;
        }
        if(backups.at(i) == lastBackup){
            end=i;
            found=true;
            break;
        }
    }
    if(found == false) {throw std::invalid_argument("backup " + lastBackup.string() + " not found in backups.log");}
    backups.assign(backups.begin()+lastFullBackup, backups.begin()+end+1);

    return backups;

}

std::vector<fs::path> findRecentBackups(fs::path path){
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

std::string getFileTree(fs::path path, bool includeHiddenFiles=false){
    //start fileTree with relative path so can substr further elements if desired
    std::string fileTree = path.string() + "\n";
    std::vector<fs::path> folderQueue;
    folderQueue.push_back(path);
    fs::path currentPath;
    while(!folderQueue.empty()){
        currentPath = folderQueue.at(0);
        folderQueue.erase(folderQueue.begin());
        for(const  fs::path & entry : fs::directory_iterator(currentPath)){
            if(!includeHiddenFiles && entry.filename().string()[0] == '.'){
                continue;
            }
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
void saveFileTree(fs::path inputPath, fs::path destination, bool includeHiddenFiles=false){
    //creates folder to store destination if doesn't exist
    std::string tree = getFileTree(inputPath, includeHiddenFiles);
    createParentFolderIfDoesntExist(destination);
    writeToFile(destination, tree);
}
//all files should be in same folder, because the output of the tar file will be in the folder containing file 0 in files
std::string compressAndDeleteFiles(std::vector<fs::path> files){
    int i=0;
    std::string backupLocation = files.at(0).parent_path() / "0.tar";
    while(doesPathExist(backupLocation)){
        i++;
        backupLocation = files.at(0).parent_path() / (std::to_string(i)+".tar");
    }
    std::string command = "tar czf " + backupLocation + " -C" + files.at(0).parent_path().string();
    for(fs::path file :files){
        command += " " + file.filename().string();

    }
    command += " >/dev/null";
    system(command.c_str());

    for(fs::path file : files){
        fs::remove(file);
    }
    return backupLocation;
}

//takes tarSize as long in bytes
void compressBackupDirectory(fs::path backupLocation, fs::path backupMapFile, long tarSize){
    std::string backupMap = "";
    std::vector<fs::path> folderQueue;
    folderQueue.push_back(backupLocation);
    fs::path currentPath;
    std::vector<std::vector<fs::path>> compressionFileList;
    while(!folderQueue.empty()){
        currentPath = folderQueue.at(0);
        folderQueue.erase(folderQueue.begin());
        std::vector<fs::path> nonDirectoryVec;

        for(const fs::path & entry : fs::directory_iterator(currentPath)){
            if(is_directory(entry)){
                folderQueue.push_back(entry);
            }
            else{
                nonDirectoryVec.push_back(entry);
            }
        }

        int sum=0;
        std::vector<fs::path> compressionFiles;
        if(!nonDirectoryVec.empty()){
            std::string relativePath = currentPath.string().substr(backupLocation.string().size());
            if(relativePath.size() == 0) {
                relativePath = "";
            }
            else{
                relativePath=relativePath.substr(1);
            }
        }
        while(!nonDirectoryVec.empty()){
            sum+=std::filesystem::file_size(nonDirectoryVec.at(0));
            compressionFiles.push_back(nonDirectoryVec.at(0));
            nonDirectoryVec.erase(nonDirectoryVec.begin()); 
            if(sum >= tarSize){
                compressionFileList.push_back(compressionFiles);
                compressionFiles = std::vector<fs::path>();
            }
        }
        if(!compressionFiles.empty()){
            compressionFileList.push_back(compressionFiles);
        }
    }
    for(unsigned int i=0; i<compressionFileList.size();i++){
        std::string compressionName = compressAndDeleteFiles(compressionFileList.at(i));
        std::string relativePath = compressionFileList.at(i).at(0).parent_path().string().substr(backupLocation.string().size());
        if(relativePath.size() == 0) {
            relativePath = "";
        }
        else{
            relativePath=relativePath.substr(1);
        }
        backupMap += relativePath + "\n";
        
        backupMap += compressionName.substr(backupLocation.string().size()+1);
        for(fs::path file : compressionFileList.at(i)){
            backupMap += "," + file.string().substr(backupLocation.string().size()+1);    
        }
        backupMap+="\n";
    }


    //std::cout<<backupMapFile<<std::endl;
    createParentFolderIfDoesntExist(backupMapFile);
    writeToFile(backupMapFile, backupMap);
}


void setupSystemdTimer(){
    std::string homeDir = std::getenv("HOME");
    std::string serviceLoc = homeDir + "/.config/systemd/user/TimeMachine.service";
    std::string timerLoc = homeDir+ "/.config/systemd/user/TimeMachine.timer";
    
    std::string serviceStr = R""""(
[Unit]
Description=TimeMachine is a differential backup system

[Service]
ExecStart=/usr/local/bin/timeMachineCLI
Type=simple


[Install]
WantedBy=default.target
    )"""";

    std::string timerStr = R""""(
[Unit]
Description=Schedule timeMachineCLI to be ran repeatedly 

[Timer]
OnBootSec=15min
OnUnitActiveSec=1h

[Install]
WantedBy=timers.target
    )"""";


    writeToFile(serviceLoc, serviceStr);
    writeToFile(timerLoc, timerStr);

    system("systemctl --user daemon-reload");
    system("systemctl --user start TimeMachine.timer");
}

void setupConfigFile(std::string inputLoc, std::string outputLoc, int schedule = 3600, int backupRatio = 50, bool compression=true){
    std::string text = "[input]\n"
            +inputLoc
            +"\n[output]\n"
            +outputLoc
            +"\n[schedule]\n"
            +std::to_string(schedule)
            +"\n[backupRatio]\n"
            +std::to_string(backupRatio)
            +"\n[compression]\n";
    if (compression)text+="true";
    else text+="false";
    std::string configLoc = std::string(std::getenv("HOME")) + "/.config/TimeMachine/config.conf";
    createParentFolderIfDoesntExist(configLoc);
    writeToFile(configLoc, text);
}




#endif

