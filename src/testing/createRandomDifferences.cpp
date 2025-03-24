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

int main(int argc, char* argv[]){
    srand(time(NULL));
    
    startPoint =0; 
    if ( argc > 4){
        if(argv[2] == "-C")   
        {
            startPoint =std::stoi(argv[3]); 
        }
    }
    //5GB target file size 

    if(argc < 2) {return -1;}

    fs::path path = argv[1];

    int stepsToMake = 75;
    fs::path prevLow;
    fs::path prevMed;
    fs::path prevHigh;
    if(startPoint ==0){

        prevLow = path/"start";
        prevMed = prevLow;
        prevHigh = prevLow;
    
        fs::remove_all(path/"low");
        fs::remove_all(path/"med");
        fs::remove_all(path/"high");
        fs::create_directory(path/"low");
        fs::create_directory(path/"med");
        fs::create_directory(path/"high");
    }
    else{
        prevLow = path/"low"/std::to_string(startPoint);
        prevMed = path/"med"/std::to_string(startPoint);
        prevHigh = path/"high"/std::to_string(startPoint);
    }

    for(int i=startPoint; i<stepsToMake; i++){
        //random between 25MB and 75MB
        unsigned int diff = rand() % 50000000 + 25000000;
        fs::path newLow = path/"low"/std::to_string(i);
        fs::path newMed = path/ "med" / std::to_string(i);
        fs::path newHigh = path/ "high"/ std::to_string(i);
        fs::copy(prevLow, newLow,fs::copy_options::recursive);
        fs::copy(prevMed, newMed,fs::copy_options::recursive);
        fs::copy(prevHigh, newHigh, fs::copy_options::recursive);
        changeFileSystem(newLow,diff);
        //random between 100MB and 200MB
        diff = rand() % 100000000 + 100000000;
        changeFileSystem(newMed,diff);
        //random between 400MB and 800MB 
        diff = rand() % 400000000 + 400000000;
        changeFileSystem(newHigh,diff);
        prevLow = newLow;
        prevMed = newMed;
        prevHigh = newHigh;
    }

}

//returns int containing size of file created
int changeFile(fs::path path, unsigned int storageAmount){
    std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
	 
    unsigned int amountWritten =0;
    unsigned int batchSize = storageAmount/2;
    if(batchSize > storageAmount/2){batchSize =storageAmount/2;}

    unsigned int amountRemoved =0;

    std::ifstream input_file(path);

    std::string text="";
    std::string line;
    while(getline(input_file, line)){
        text+=line;
    }

    int j=rand();
    while(fs::exists(path.parent_path()/std::to_string(j))){j=rand();}
    std::ofstream output_file(path.parent_path()/std::to_string(j));
    int changedData =0; 
    if( input_file && output_file){
        if((storageAmount/2) >= text.size()){
            changedData += text.size();
            text ="";
            
        }

        while(amountRemoved < storageAmount/2)
        {
            for(unsigned int i=0; i<text.size()-100;i+=30){
                if(i > text.size() -100){break;}
                int num = rand()%100;
                if(num < 5){
                    text = text.substr(0,i)+ text.substr(i+100);
                    amountRemoved +=100;
                }
            }
        }
        output_file << text;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        fs::remove(path);
        fs::rename(path.parent_path()/ std::to_string(j), path);
    }

    

    while(amountWritten < storageAmount/2)
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
    changedData+=amountWritten;
    return changedData;
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
    long averageChange = 0;

    if(targetSize <  100000000){
        averageChange = targetSize/100;
    }
    else if (targetSize < 4000000){
        averageChange = targetSize/200;
    }
    else{
        averageChange = targetSize/400;
    }

    while(bytesLeft>0){
        int num = rand() % files.size();
        int changeAmount = rand() % averageChange;
        bytesLeft -= changeFile(files.at(num), changeAmount);
    }

}
