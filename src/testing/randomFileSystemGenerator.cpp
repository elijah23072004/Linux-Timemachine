#include <iostream>
#include <stdlib.h>
#include <filesystem>
#include <time.h>
#include <fstream>

namespace fs = std::filesystem;

int createFile(fs::path path);
void createFileSystem(fs::path path, long targetSize);

int main(int argc, char* argv[]){
    srand(time(NULL));

    //5GB target file size 
    long targetSize = 2500000000;

    if(argc < 2) {return -1;}

    fs::path destination = argv[1];
    createFileSystem(destination, targetSize);
}

//returns int containing size of file created
int createFile(fs::path path){
    int num = rand() % 100;
    int fileSize;
    if (num <=4){
        fileSize = 30000000; 
    }
    else if (num <=19){
        fileSize = 5000000;
    }
    else if (num <= 97){
        fileSize = 100000;
    }
    else{
        fileSize = 250000000;
    }
    std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
	 
    int amountWritten =0;
    int batchSize = fileSize;

    fs::remove(path);

    while(amountWritten < fileSize)
    {

        std::ofstream myfile;
        myfile.open(path, std::ios_base::app);
        

        std::string text = "";
        for (int i=0; i<batchSize; i++){
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
    return fileSize;
}


void createFileSystem(fs::path path, long targetSize)
{
    fs::create_directory(path);
    fs::path currentPath = path;
    unsigned int i=0;
    long bytesLeft = targetSize;
    bool lastActionNewDirectory = true;
    while(bytesLeft >0){
        // 10% new folder, 70% file 20% ..
        int num = rand() % 10;
        if(lastActionNewDirectory == true){
            num = 5;
            lastActionNewDirectory=false;
        }
        if (num <1){
            currentPath /= std::to_string(i);
            fs::create_directory(currentPath);
            i++;
            lastActionNewDirectory=true;
        }
        else if(num<7){
            bytesLeft -= createFile(currentPath/std::to_string(i));
            i++;
        }
        else{
            if(currentPath == path){
                currentPath /=std::to_string(i);
                i++;
                fs::create_directory(currentPath);
                lastActionNewDirectory=true;
            }
            else {
                currentPath = currentPath.parent_path();
                lastActionNewDirectory=true;
            }
        }

    }
}
