#include <iostream>
#include <string>
#include "utils.cpp"
#include "fullBackup.cpp"
#include "differentialBackup.cpp"
#define PROJECT_NAME "linuxTimeMachine"

int main(int argc, char **argv) {

    if(argc != 4) {
        recoverFileTest();
        std::cout << argv[0] <<  "requires 4 arguements\n";
        return 1;
    }
    std::cout << "This is project " << PROJECT_NAME << ".\n";
    std::string backupMode = argv[3]; 
    if(backupMode == "-f"){
        std::cout<<fullBackup(argv[1],argv[2])<<std::endl;
    }
    else if (backupMode == "-d"){
        std::cout<<differentialBackup(argv[1], argv[2])<<std::endl;
    }
    else{
        std::cout<<"invalid third arguement"<<std::endl;
    }
    

    return 0;
}
