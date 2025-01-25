#include <iostream>
#include "fullBackup.cpp"
#include "differentialBackup.cpp"
#define PROJECT_NAME "linuxTimeMachine"

int main(int argc, char **argv) {
    if(argc != 3) {
        std::cout << argv[0] <<  "requires 3 arguements\n";
        return 1;
    }
    std::cout << "This is project " << PROJECT_NAME << ".\n";
    
    std::cout<<fullBackup(argv[1], argv[2])<<std::endl;
    
    changedFileTester(argv[1]);
    

    return 0;
}
