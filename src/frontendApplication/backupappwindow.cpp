#include "backupappwindow.h"
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <vector>
#include <string>
#include <ctime>

#include <set>
#include "../utils.cpp"

namespace fs = std::filesystem;


BackupAppWindow::BackupAppWindow(BaseObjectType* cobject, 
                                 const Glib::RefPtr<Gtk::Builder>& refBuilder)
 :  Gtk::ApplicationWindow(cobject),
    m_refBuilder(refBuilder)
{
    //loads widgets from ui file to window
    auto menu = m_refBuilder->get_widget<Gtk::Grid>("menuButtons");
    if(!menu)
        throw std::runtime_error("No \"menuButtons\" object in mainWindow.ui");
    auto backupGrid = m_refBuilder->get_widget<Gtk::Grid>("backupGrid");
    if(!backupGrid)
        throw std::runtime_error("No \"backupGrid\" object in mainWindow.ui");
    auto lastBackupGrid = m_refBuilder->get_widget<Gtk::Box>("lastBackups");
    if(!lastBackupGrid)
        throw std::runtime_error("No \"lastBackups\" object in mainWindow.ui");

    //Get windgets from the Gtk::Builder file
    m_backup = m_refBuilder->get_widget<Gtk::Button>("backup");
    if(!m_backup)
        throw std::runtime_error("No \"backup\" object in mainWindow.ui");
    m_backup->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &BackupAppWindow::backupClicked)));
    m_settings = m_refBuilder->get_widget<Gtk::Button>("settings");
    if(!m_settings)
        throw std::runtime_error("No \"edit\" object in mainWindow.ui");
    m_settings->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &BackupAppWindow::settingsClicked)));
    m_tutorial = m_refBuilder->get_widget<Gtk::Button>("tutorial");
    if(!m_tutorial)
       throw std::runtime_error("No \"tutorial\" object in mainWindow.ui");
    m_tutorial->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &BackupAppWindow::tutorialClicked)));
    m_quit = m_refBuilder->get_widget<Gtk::Button>("quit");
    if(!m_quit)
        throw std::runtime_error("No \"quit\" object in mainWindow.ui");
    m_quit->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &BackupAppWindow::quitClicked)));
    
        
     
    m_backupsScrollable = m_refBuilder->get_widget<Gtk::ScrolledWindow>("backupsScrollable");
    if(!m_backupsScrollable)
        throw std::runtime_error("No \"backupsScrollable\" object in mainWindow.ui");

   

    m_backupList = m_refBuilder->get_widget<Gtk::ListBox>("backupList");
    if(!m_backupList)
        throw std::runtime_error("No \"backups\" object in mainWindow.ui");
    
    m_filesScrollable = m_refBuilder->get_widget<Gtk::ScrolledWindow>("filesScrollable");
    if(!m_filesScrollable)
        throw std::runtime_error("No \"filesScrollable\" object in mainWindow.ui");

    m_fileTree = m_refBuilder->get_widget<Gtk::ListBox>("fileTree");
    if(!m_fileTree)
        throw std::runtime_error("No \"fileTree\" object in mainWindow.ui");


    emptyFileTree();


}

//static
BackupAppWindow* BackupAppWindow::create(BackupApplication* application)
{
    auto refBuilder = Gtk::Builder::create_from_file("/usr/lib/TimeMachine/mainWindow.ui");
    auto window = Gtk::Builder::get_widget_derived<BackupAppWindow>(refBuilder, "app_window");
    if (!window) 
        throw std::runtime_error("No \"app_window\" in mainWindow.ui");
    window->setApplication(application);
    window->checkConfigFile();
    return window;
}   

void BackupAppWindow::checkConfigFile(){
    std::string homeDir = std::getenv("HOME");
    fs::path configFile = homeDir + "/.config/TimeMachine/config.conf";
    //reads config file and if config file doesnt exist open settings window 
    //populate backups list if config file exists
    if(!fs::exists(configFile)){
        app->createSettingsWindow(this);
        return;
    }


    std::ifstream myfile(configFile.string());
    std::string line;
    while (std::getline(myfile,line)){
        if(line == "[output]"){
            std::getline(myfile,line);
            outputDir=line;
            break;
        }
    } 
    populateBackups(outputDir);


}

void BackupAppWindow::setApplication(BackupApplication* application){
    this->app = application;
}

void BackupAppWindow::quitClicked(){
    std::cout<<"quitting"<<std::endl;
    app->closeWindow("Time Machine");
}

void BackupAppWindow::backupClicked(){
    std::cout<<system("systemctl --user start TimeMachine.service")<<std::endl;
    populateBackups(outputDir);
    std::cout<<"backup"<<std::endl;
}
void BackupAppWindow::settingsClicked(){

    //setupConfigFile("/home/eli/Shared/CompSci/individualProject/projectCode/test_data/input", "/home/eli/Shared/CompSci/individualProject/projectCode/test_data/output");
    //setupSystemdTimer();
    app->createSettingsWindow(this);
    std::cout<<"settings"<<std::endl;
}


Gtk::Popover* BackupAppWindow::createPopOver(Gtk::Widget& parent, std::string text, Gdk::Rectangle pos){

    Gtk::Popover* popOver = new Gtk::Popover();
    Gtk::Label* textLabel = new Gtk::Label(text);
    Gtk::Box* popOverBox = new Gtk::Box();
    

    popOverBox->append(*textLabel);
    popOver->set_child(*popOverBox);
    popOver->set_parent(parent);

    
    popOver->set_pointing_to(pos);
    return popOver; 
}
void BackupAppWindow::tutorialClicked(){
    std::cout<<"tutorial"<<std::endl;

    Gdk::Rectangle rec = {300,50,200,50};
    std::string text = "Settings button can be clicked to bring up a window to allow customising backup options for backup program";
     //m_searchentry->signal_search_changed().connect(
  //      sigc::mem_fun(*this, &BackupAppWindow::onSearchTextChanged));


    text = text + "\n\n" + "Backup button can be clicked on to run a backup with the settings specified from the settings button";
    text = text+ "\n\n" + "Backups list on the left shows a list of all backups\nWhere a backup can be selected by clicking time and then pressing select backup button";
    text = text + "\n\n" + "File tree on right will show the files from the selected backup\nWhere you can click on folders to move directories\nAnd when on the desired directory can press restore files button to restore files from that folder";

    Gtk::Popover* popOver = createPopOver(*this,text,rec);
    popOver->popup();
    


}

std::string convertEpochToDate(long epochTime){
    tm* time = gmtime(&epochTime);
    std::string humanDate = std::asctime(time);
    return humanDate;
}

void BackupAppWindow::populateBackups(fs::path backupLocation){
    //iterates over every backup and adds a label to backuplist widget
    fs::path backupLog = backupLocation / "backups.log";
    std::string backups = readFromFile(backupLog);
    for(auto element : m_backupList->get_children()){
         m_backupList->remove(*element);
        
    }
    std::vector<std::string> backupVec = split(backups, '\n');
    Gtk::Label  label = Gtk::Label("Backups:");
    label.set_name("Backup Title");
    m_backupList->append(label);
    Gtk::Button* refresh = new Gtk::Button("Refresh backupList");
    refresh->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this,&BackupAppWindow::populateBackups),backupLocation));
    m_backupList->append(*refresh);
    Gtk::Button* select = new Gtk::Button("Select Backup");
    select->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this,&BackupAppWindow::backupSelected)));
    m_backupList->append(*select);
    for(int i=backupVec.size()-1; i>=0; i--){
        std::string backup = backupVec.at(i);
        long epochTime = std::stol(backup.substr(0,backup.length()-1));        
        label= Gtk::Label(convertEpochToDate(epochTime));
        label.set_name(backup);
        //Gtk::Button button = Gtk::Button(humanDate);
        //button.signal_clicked().connect(sigc::bind(sigc::mem_fun(*this,&BackupAppWindow::backupSelected),backup));

        m_backupList->append(label);
    }
    
}

void BackupAppWindow::emptyFileTree(){
    std::string text = "Please select a backup on the left section to allow for the files in the backup to be shown";
    Gtk::Label label = Gtk::Label(text);
    m_fileTree->append(label);
}


void BackupAppWindow::populateFileTree(fs::path fileTreeLocation, fs::path selectedPath)
{
    std::cout<<fileTreeLocation<<std::endl;
    std::string files = readFromFile(fileTreeLocation);
    std::vector<std::string> fileVec = split(files, '\n');
    std::string backupName = fileTreeLocation.filename().string();
    long epochTime = std::stol(backupName.substr(0, backupName.length()-1));
    std::cout<<epochTime<<std::endl;
    std::string date = convertEpochToDate(epochTime);
    std::string currentPath = "./" + selectedPath.string();
    Gtk::Label label = Gtk::Label("File tree: for backup: " + date + "\nCurrent path: "+ currentPath);
    
    for(auto element : m_fileTree->get_children()){
        m_fileTree->remove(*element);
    }

    m_fileTree->append(label);
    std::string relativePath = fileVec.at(0);
    fileVec.erase(fileVec.begin());


    std::vector<std::string> filteredFiles;
    for(std::string file : fileVec){
        //only show files currently in directory so ./abc but not ./1/abc:w
        std::string text = file.substr(relativePath.size());
        if (text[0] == '/'){
            text = text.substr(1);
        }

        //filter files to only have files inside selectedPath
        if(text.find(selectedPath.string())!= 0){
            continue;
        } 
        text=text.substr(selectedPath.string().size());
        if(text[0]=='/'){
            text = text.substr(1);
        }
        
        //if text contains / in path then it is in a sub folder
        if(text.find('/') != std::string::npos){
            int index = text.find('/');
            text[index]='\\';
            if(text.substr(index).find('/') != std::string::npos){
                //if in 2 levels of subfolders then dont append to list since 
                //file inside desired folder already found
                continue;
            }
            text[index]='/';
            text = text.substr(0,text.find('/'));
            //add file to vector with / at end to symbolise a folder            
            //
            filteredFiles.push_back(text  +"/");

            continue;
        }
        filteredFiles.push_back(text);

    }
    //remove duplicates (like folder in twice or abc and abc/ both in)
    for(unsigned int i=0; i<filteredFiles.size();i++){
        std::string toFind = filteredFiles.at(i);
        bool isFolder=false;
        if(toFind.size() == 0){continue;}
        if(toFind[toFind.size()-1] == '/'){
            toFind=toFind.substr(0,toFind.size()-1);
            isFolder=true;
        }
        unsigned int j=i+1;
        while(j<filteredFiles.size()){
            if(filteredFiles.at(j) == toFind){
                filteredFiles.erase(filteredFiles.begin() + j); 
                isFolder=true;
            }
            else if(filteredFiles.at(j) == (toFind + '/')){
                filteredFiles.erase(filteredFiles.begin() + j);
                isFolder=true;
            }
            else{ j++;}

        }
        if(isFolder){
            filteredFiles.at(i) = toFind + '/';
        }
    }
    //sort files with folders at top  
    std::vector<std::string> folders;
    std::vector<std::string> nonFolders; 
    if(selectedPath != ""){
        folders.push_back("../");
    }
    for(std::string file: filteredFiles){
        if(file.size() == 0){continue;}
        if(file[file.size()-1] == '/'){
            folders.push_back(file);
        }
        else{
            nonFolders.push_back(file);
        }
    }

    std::vector<std::string> combined = folders; 
    if(!nonFolders.empty()){
        combined.insert(combined.end(), nonFolders.begin(), nonFolders.end());
    }
    Gtk::Button* button =new  Gtk::Button("Restore Files");
    fs::path backupLocation =fileTreeLocation.parent_path().parent_path() / backupName; 
    for(unsigned int i=0; i<combined.size();i++){
        combined.at(i) = (selectedPath/combined.at(i)).string();
    }
    //get rid of ../ entry
    std::cout<<"combined size: " <<combined.size()<<std::endl;
    if(combined.size() != 0 && combined.at(0) == "../"){
        combined.erase(combined.begin());
    }
    
    button->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &BackupAppWindow::restoreFiles),combined, backupLocation ));
    m_fileTree->append(*button);


    for(std::string filename : folders){
        Gtk::Button* button = new Gtk::Button(filename);
        button->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &BackupAppWindow::traversedFileTree),selectedPath.string(), filename ));
        m_fileTree->append(*button);
    }
    for(std::string filename: nonFolders){
        label = Gtk::Label(filename);
        m_fileTree->append(label); 
    }
}

void BackupAppWindow::backupSelected(){
    std::cout<<"backup selected: "<<std::endl;
    std::cout<<m_backupList->get_selected_row()<<std::endl;
    if(m_backupList->get_selected_row() == 0){return;}
    std::vector<Gtk::Widget*> selected = m_backupList->get_selected_row()->get_children();
    std::cout<<selected.at(0)->get_name()<<std::endl;
    if(selected.at(0)->get_name()=="Backup Title"){

        return;
    }
    currentSelectedBackup = outputDir/"fileTrees"/std::string(selected.at(0)->get_name());
    //populateFileTree(outputDir/"fileTrees"/std::string(selected.at(0)->get_name()), fs::path(""));
    populateFileTree(currentSelectedBackup, fs::path(""));
}

void BackupAppWindow::setElementWidths(){
    int windowWidth= 0;
    windowWidth = this->get_width();
    int windowHeight = this->get_height();
    int buttonWidth = windowWidth/3; 
    m_backup->set_size_request(buttonWidth, -1);
    m_settings->set_size_request(buttonWidth, -1);
    m_tutorial->set_size_request(buttonWidth,-1);
    m_backupsScrollable->set_size_request(windowWidth*0.33, -1);
    m_backupList->set_size_request(windowWidth*0.33, -1);
    m_filesScrollable->set_size_request(windowWidth*0.66,windowHeight*0.9);
    m_fileTree->set_size_request(windowWidth*0.66,windowHeight*0.9);

}

void BackupAppWindow::size_allocate_vfunc(int width, int height, int baseline){
    Gtk::Widget::size_allocate_vfunc(width, height, baseline);
    setElementWidths();
}


//void BackupAppWindow::onSearchTextChanged(){
//    std::cout<<"search text changed"<<std::endl;
//}

void BackupAppWindow::traversedFileTree(fs::path selectedPath, std::string fileName){
    std::cout<<"button clicked"<<std::endl;
    std::cout<<fileName<<std::endl;
    std::cout<<"current selected backup: " << currentSelectedBackup.string()<<std::endl;
    if(fileName == "../"){
        std::cout<<"selected path is " << selectedPath.string()<<std::endl;
        //if last char in selected path is / then need to do 2 parent paths to
        //move up one directory;
        if(selectedPath.string()[selectedPath.string().size()-1] == '/'){
            selectedPath=selectedPath.parent_path();
        }
        selectedPath = selectedPath.parent_path(); 
        if(selectedPath.string () != ""){
            selectedPath +="/";
        }
        std::cout<<"selected path is " << selectedPath.string()<<std::endl;
    }
    else{
        selectedPath = selectedPath / fileName;
    }
    populateFileTree(currentSelectedBackup, selectedPath);
}

void BackupAppWindow::restoreFiles(std::vector<std::string> files, std::filesystem::path backupLocation){
    std::cout<<backupLocation.string()<<std::endl;
    //show window allowing for selection of files to recover 
    app->createRecoverWindow(files, backupLocation);
    
}

