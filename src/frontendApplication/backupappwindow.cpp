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
    populateFileTree(outputDir/"fileTrees"/lastBackup);

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
        tm* time = gmtime(&epochTime);
        std::string humanDate = std::asctime(time);
        label= Gtk::Label(humanDate);
        label.set_name(backup);
        //Gtk::Button button = Gtk::Button(humanDate);
        //button.signal_clicked().connect(sigc::bind(sigc::mem_fun(*this,&BackupAppWindow::backupSelected),backup));

        m_backupList->append(label);
    }
    

}

void BackupAppWindow::populateFileTree(fs::path fileTreeLocation)
{
    std::string files = readFromFile(fileTreeLocation);
    std::vector<std::string> fileVec = split(files, '\n');
    Gtk::Label label = Gtk::Label("File tree: \nCurrent path: .");
    
    for(auto element : m_fileTree->get_children()){
        m_fileTree->remove(*element);
    }

    m_fileTree->append(label);
    std::string relativePath = fileVec.at(0);
    fileVec.erase(fileVec.begin());
    for(std::string file : fileVec){
        //only show files currently in directory so ./abc but not ./1/abc:w
        std::string text = "./" + file.substr(relativePath.size());
        
        label = Gtk::Label(text);
        m_fileTree->append(label);
    }
}

void BackupAppWindow::backupSelected(){
    std::cout<<"backup selected: "<<std::endl;
    std::cout<<m_backupList->get_selected_row()<<std::endl;
    std::vector<Gtk::Widget*> selected = m_backupList->get_selected_row()->get_children();
    std::cout<<selected.at(0)->get_name()<<std::endl;


    populateFileTree(outputDir/"fileTrees"/std::string(selected.at(0)->get_name()));
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
