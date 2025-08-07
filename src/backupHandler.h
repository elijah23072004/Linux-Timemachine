#ifndef BACKUPHANDLER
#define BACKUPHANDLER
#include <string>
#include<filesystem>
#include<vector>
namespace fs = std::filesystem;
class BackupHandler{
    public:
    BackupHandler();
    BackupHandler(int argc, char** argv);
    int getOutputCode();

    private:
    fs::path configFileLocation;
    fs::path inputLocation;
    fs::path outputLocation;
    fs::path backupFolderLocation;
    bool compression=false;
    bool includeHiddenFiles=false;
    char backupMode='0';
    int backupRatio;
    int outputCode=0;
    void setOutputCode(int code); 
    void parseConfigFile();
    void handleArguments(int argc, char** argv);
    void executeBackup(); 
    void outputHelp();
};

#endif
