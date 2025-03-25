#include <iostream>
#include <stdlib.h>
#include <filesystem>
#include <time.h>
#include <fstream>
#include "../utils.cpp"
#include <chrono>
#include <thread>

namespace fs = std::filesystem;

int changeFile(fs::path path, unsigned int storageAmont);
void changeFileSystem(fs::path path,  unsigned int targetSize);

void createChangedFiles(fs::path path, unsigned int changeAmount){
    unsigned int diff = rand() % changeAmount+ changeAmount;
    changeFileSystem(path,diff);
}
int main(int argc, char* argv[]){
    srand(time(NULL));
    

    if(argc < 3) {return -1;}

    fs::path path = argv[1];
    int changeAmount = std::stoi(argv[2]);
    
    int threads = 8;
    std::vector<std::thread> jobs;

    for(int i=0 ;i<threads;i++){
        jobs.push_back(std::thread(createChangedFiles,path, changeAmount/8)); 
    }
    for(int i=0; i<threads;i++){
        jobs.at(i).join();
    }



}

//returns int containing size of file created
int changeFile(fs::path path, unsigned int storageAmount){
    std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
	 
    unsigned int amountWritten =0;
    unsigned int batchSize = storageAmount;

    while(amountWritten < storageAmount)
    {

        std::ofstream myfile;
        myfile.open(path, std::ios_base::app);
        

        std::string text = "";
        for (unsigned int i=0; i<batchSize; i++){
            int num = rand(); 
            if((num % 500 == 0 )){
                text += "\n";
            }
            text += characters[num%characters.length()];

        }
        amountWritten+=batchSize;
        myfile << text;
        text="";


    }
    return amountWritten;
}


void changeFileSystem(fs::path path, unsigned int targetSize)
{
    std::vector<fs::path> files;
    std::vector<fs::path> folderQueue;
    folderQueue.push_back(path);
    fs::path currentPath;
    while(!folderQueue.empty()){
        currentPath = folderQueue.at(0);
        folderQueue.erase(folderQueue.begin());
        for(const  fs::path & entry : fs::directory_iterator(currentPath)){
            if(is_directory(entry)){
                folderQueue.push_back(entry);
            }
            else{
                files.push_back(entry);
            }
        }
    }
    
    long bytesLeft = targetSize;
    long averageChange = 200;


    while(bytesLeft>0){
        int num = rand() % files.size();
        int changeAmount = rand() % averageChange;
        bytesLeft -= changeFile(files.at(num), changeAmount);
    }

}
