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
    m_log = m_refBuilder->get_widget<Gtk::Button>("log");
    if(!m_log)
        throw std::runtime_error("No \"log\" object in mainWindow.ui");
    m_log->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &BackupAppWindow::logClicked)));
    m_backup = m_refBuilder->get_widget<Gtk::Button>("backup");
    if(!m_backup)
        throw std::runtime_error("No \"backup\" object in mainWindow.ui");
    m_backup->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &BackupAppWindow::backupClicked)));
    m_restore = m_refBuilder->get_widget<Gtk::Button>("restore");
    if(!m_restore)
        throw std::runtime_error("No \"restore\" object in mainWindow.ui");
    m_restore->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &BackupAppWindow::restoreClicked)));
    m_edit = m_refBuilder->get_widget<Gtk::Button>("edit");
    if(!m_edit)
        throw std::runtime_error("No \"edit\" object in mainWindow.ui");
    m_edit->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &BackupAppWindow::editClicked)));
    m_settings = m_refBuilder->get_widget<Gtk::Button>("settings");
    if(!m_settings)
        throw std::runtime_error("No \"settings\" object in mainWindow.ui");
    m_settings->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &BackupAppWindow::settingsClicked)));
    m_tutorial = m_refBuilder->get_widget<Gtk::Button>("tutorial");
    if(!m_tutorial)
       throw std::runtime_error("No \"tutorial\" object in mainWindow.ui");
    m_tutorial->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &BackupAppWindow::tutorialClicked)));
    m_quit = m_refBuilder->get_widget<Gtk::Button>("quit");
    if(!m_quit)
        throw std::runtime_error("No \"quit\" object in mainWindow.ui");
    m_quit->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &BackupAppWindow::quitClicked)));
    
    m_backupLabel = m_refBuilder->get_widget<Gtk::Label>("backupLabel");
    if(!m_backupLabel)
        throw std::runtime_error("No \"backupLabel\" object in mainWindow.ui");

    m_searchentry = m_refBuilder->get_widget<Gtk::SearchEntry>("backupSearchEntry");
    if(!m_searchentry)
        throw std::runtime_error("No \"backupSearchEntry\" object in window.ui");

    m_searchbar = m_refBuilder->get_widget<Gtk::SearchBar>("backupSearch");
    if(!m_searchbar)
        throw std::runtime_error("No \"backupSearch\" object in mainWindow.ui");

    m_backupList = m_refBuilder->get_widget<Gtk::ListBox>("backupList");
    if(!m_backupList)
        throw std::runtime_error("No \"backups\" object in mainWindow.ui");
    m_fileTree = m_refBuilder->get_widget<Gtk::ListBox>("fileTree");
    if(!m_fileTree)
        throw std::runtime_error("No \"fileTree\" object in mainWindow.ui");

    m_searchbar->connect_entry(*m_searchentry);
    m_searchbar->set_search_mode(true);

    m_searchentry->signal_search_changed().connect(
        sigc::mem_fun(*this, &BackupAppWindow::onSearchTextChanged));
    //need to set widths and locations of log,backup,restore,edit,tutorial buttons so fit top width of screen 
    //row->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this,
    //  &ExampleAppWindow::on_find_word), row));
   
    outputDir = "./test_data/output";
    populateBackups(outputDir);

    std::string tmp = readFromFile(outputDir/"backups.log");
    
    std::vector<std::string> backups = split(tmp,'\n');
    std::string lastBackup = backups.at(backups.size()-1);
    currentSelectedBackup = outputDir / "fileTrees"/lastBackup;
    populateFileTree(currentSelectedBackup, fs::path(""));

}

//static
BackupAppWindow* BackupAppWindow::create()
{
    auto refBuilder = Gtk::Builder::create_from_file("src/frontendApplication/mainWindow.ui");
    auto window = Gtk::Builder::get_widget_derived<BackupAppWindow>(refBuilder, "app_window");
    if (!window) 
        throw std::runtime_error("No \"app_window\" in mainWindow.ui");
    return window;
}   


void BackupAppWindow::quitClicked(){
    std::cout<<"quitting"<<std::endl;
}

void BackupAppWindow::logClicked(){
    setElementWidths();
    std::cout<<"log"<<std::endl;
}
void BackupAppWindow::backupClicked(){
    std::cout<<system("linuxTimeMachineCLI")<<std::endl;
    std::cout<<"backup"<<std::endl;
}
void BackupAppWindow::restoreClicked(){
    std::cout<<"restore"<<std::endl;
}
void BackupAppWindow::editClicked(){
    std::cout<<"edit"<<std::endl;
}
void BackupAppWindow::settingsClicked(){

    setupConfigFile("/home/eli/Shared/CompSci/individualProject/projectCode/test_data/input", "/home/eli/Shared/CompSci/individualProject/projectCode/test_data/output");
    setupSystemdTimer();

    std::cout<<"settings"<<std::endl;
}
void BackupAppWindow::tutorialClicked(){
    setElementWidths();
    std::cout<<"tutorial"<<std::endl;
}

std::string convertEpochToDate(long epochTime){
    tm* time = gmtime(&epochTime);
    std::string humanDate = std::asctime(time);
    return humanDate;
}

void BackupAppWindow::populateBackups(fs::path backupLocation){
    fs::path backupLog = backupLocation / "backups.log";
    std::string backups = readFromFile(backupLog);
    
    std::vector<std::string> backupVec = split(backups, '\n');
    Gtk::Label  label = Gtk::Label("Backups:");
    m_backupList->append(label);
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
            text = text.substr(0,text.find('/'));
            //add file to vector with / at end to symbolise a folder            
            filteredFiles.push_back(text  +"/");

            continue;
        }
        filteredFiles.push_back(text);

    }
    std::cout<<filteredFiles.size()<<std::endl;
    //remove duplicates (like folder in twice or abc and abc/ both in)
    for(unsigned int i=0; i<filteredFiles.size();i++){
        std::string toFind = filteredFiles.at(i);
        bool isFolder=false;
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
        if(file[file.size()-1] == '/'){
            folders.push_back(file);
        }
        else{
            nonFolders.push_back(file);
        }
    }
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
    std::vector<Gtk::Widget*> selected = m_backupList->get_selected_row()->get_children();
    std::cout<<selected.at(0)->get_name()<<std::endl;

    currentSelectedBackup = outputDir/"fileTrees"/std::string(selected.at(0)->get_name());
    //populateFileTree(outputDir/"fileTrees"/std::string(selected.at(0)->get_name()), fs::path(""));
    populateFileTree(currentSelectedBackup, fs::path(""));
}

void BackupAppWindow::setElementWidths(){
    int windowWidth= 0;
    //int windowHeight=0;
    //this->get_default_size(windowWidth, windowHeight);
    //std::cout<<windowWidth<<std::endl;
    windowWidth = this->get_width();
    int buttonWidth = windowWidth/5; 
    //std::cout<<windowWidth<<std::endl;
    //std::cout<<m_log->get_width()<<std::endl;
    m_log->set_size_request(buttonWidth, -1);
    m_backup->set_size_request(buttonWidth, -1);
    m_restore->set_size_request(buttonWidth, -1);
    m_edit->set_size_request(buttonWidth, -1);
    m_tutorial->set_size_request(buttonWidth,-1);
    /*std::cout<<buttonWidth<<std::endl;
   
    std::cout<<"log: " << m_log->get_width()<<std::endl;
    
    std::cout<<"backup: " << m_backup->get_width()<<std::endl;
    
    std::cout<<"restore: " << m_restore->get_width()<<std::endl;
    
    std::cout<<"edit: " << m_edit->get_width()<<std::endl;
    
    std::cout<<"tutorial: " << m_tutorial->get_width()<<std::endl;
    */
    m_backupList->set_size_request(windowWidth*0.33, -1);
    m_fileTree->set_size_request(windowWidth*0.66,-1);

}

void BackupAppWindow::size_allocate_vfunc(int width, int height, int baseline){
    Gtk::Widget::size_allocate_vfunc(width, height, baseline);
    setElementWidths();
}


void BackupAppWindow::onSearchTextChanged(){
    std::cout<<"search text changed"<<std::endl;
}

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

