#include "settingsappwindow.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <set>


namespace fs = std::filesystem;

SettingsAppWindow::SettingsAppWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
  : Gtk::ApplicationWindow(cobject),
    m_refBuilder(refBuilder)
{

    auto contentGrid = m_refBuilder->get_widget<Gtk::Grid>("contentGrid");
    if(!contentGrid)
        throw std::runtime_error("No \"backupGrid\" object in settings.ui");
    auto headingLabel = m_refBuilder->get_widget<Gtk::Label>("headingLabel");
    if(!headingLabel)
        throw std::runtime_error("No \"headingLabel\" object in settings.ui");
    m_inputLabel = m_refBuilder->get_widget<Gtk::Label>("inputLabel");
    if(!m_inputLabel)
        throw std::runtime_error("No \"inputLabel\" object in settings.ui");

    m_inputFileDialogButton = m_refBuilder->get_widget<Gtk::Button>("inputFileDialogButton");
    if(!m_inputFileDialogButton)
        throw std::runtime_error("No \"inputFileDialogButton\" object in settings.ui");
    m_inputFileDialogButton->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &SettingsAppWindow::openFileDialog),true));
        

    m_outputLabel = m_refBuilder->get_widget<Gtk::Label>("outputLabel");
    if(!m_outputLabel)
        throw std::runtime_error("No \"outputLabel\" object in settings.ui");

    m_outputFileDialogButton = m_refBuilder->get_widget<Gtk::Button>("outputFileDialogButton");
    if(!m_outputFileDialogButton)
        throw std::runtime_error("No \"outputFileDialogButton\" object in settings.ui");
    m_outputFileDialogButton->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &SettingsAppWindow::openFileDialog),false));
        
    m_compression = m_refBuilder->get_widget<Gtk::CheckButton>("compression");
    if(!m_compression) 
        throw std::runtime_error("No \"compression\" object in settings.ui");
    m_compression->set_active(true);
    m_schedule = m_refBuilder->get_widget<Gtk::CheckButton>("schedule");
    if(!m_schedule)
        throw std::runtime_error("No \"schedule\" object in settings.ui");
    m_schedule->set_active(true);

    m_advanced = m_refBuilder->get_widget<Gtk::Label>("advanced");
    if(!m_advanced)
        throw std::runtime_error("No \"advanced\" object in settins.ui");

    m_backupRatio = m_refBuilder->get_widget<Gtk::Text>("backupRatio");
    if(!m_backupRatio)
        throw std::runtime_error("No \"backupratio\" object in settings.ui");

    m_cancel = m_refBuilder->get_widget<Gtk::Button>("cancel");
    if(!m_cancel)
        throw std::runtime_error("No \"restore\" object in settings.ui");
    m_cancel->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &SettingsAppWindow::cancelClicked)));

    m_save = m_refBuilder->get_widget<Gtk::Button>("save");
    if(!m_save)
        throw std::runtime_error("No \"restore\" object in settings.ui");
    m_save->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &SettingsAppWindow::saveClicked)));
    
    m_errLabel = m_refBuilder->get_widget<Gtk::Label>("errLabel");
    if(!m_errLabel)
        throw std::runtime_error("No \"errLabel\" object in settings.ui");

    autoFillInputs();
}

void SettingsAppWindow::autoFillInputs(){
    std::string homeDir = std::getenv("HOME");
    fs::path configFile = homeDir + "/.config/TimeMachine/config.conf";

    if(!fs::exists(configFile)){return;}

    std::ifstream myfile(configFile.string());
    std::string line;
    m_schedule->set_active(false);
    while (std::getline(myfile,line)){
        if(line == "[input]"){
            std::getline(myfile,line);
            inputPath=line;
            m_inputLabel->set_label("Input folder: " + line);
        }
        else if(line == "[output]"){
            std::getline(myfile,line);
            outputPath=line;
            m_outputLabel->set_label("Output folder: " + line);
        }
        else if(line=="[backupRatio]"){
            std::getline(myfile,line);
            m_backupRatio->get_buffer()->set_text(line);
        }
        else if(line == "[compression]"){
            std::getline(myfile,line);
            if(line == "false"){
               m_compression->set_active(false); 
            }
        }
        else if(line =="[schedule]"){
            std::getline(myfile,line);
            m_schedule->set_active(true);
        }
    }
    
    
}

SettingsAppWindow* SettingsAppWindow::create()
{
    auto refBuilder = Gtk::Builder::create_from_file("src/frontendApplication/settings.ui");
    auto window = Gtk::Builder::get_widget_derived<SettingsAppWindow>(refBuilder, "app_window");
    if (!window) 
        throw std::runtime_error("No \"app_window\" in settings.ui");
    return window;
}

void SettingsAppWindow::setApplication(BackupApplication* app){
    this->app = app;
}
void SettingsAppWindow::saveClicked(){
    bool schedule = false;
    std::string text = "[input]\n";
    if(inputPath == ""){
        std::string errMessage = "Please select input folder for backup";
        m_errLabel->set_label(errMessage);
        return;
    }
    text+=inputPath;
    text+="\n[output]\n";
    if(outputPath==""){
        std::string errMessage="Please select output folder for backup";
        m_errLabel->set_label(errMessage);
        return;
    }
    text+=outputPath;
    if(m_compression->get_active()){
        text+="\n[schedule]\n3600";
        schedule=true;
    }
    if(m_backupRatio->get_buffer()->get_text() != ""){
        std::string ratioText = m_backupRatio->get_buffer()->get_text();
        for(unsigned int i=0; i<ratioText.size();i++){
            if(!std::isdigit(ratioText[i])){
                std::string errMessage = "Please only type digits into backupRatio";
                m_errLabel->set_label(errMessage);
                m_backupRatio->get_buffer()->delete_text(0,-1);
                return;
            }
        }
        text+="\n[backupRatio]\n"+ratioText;
    }
    else{
        text+="\n[backupRatio]\n50";
    }
    if(m_compression->get_active()){
        text+="\n[compression]\ntrue";
    }
    else{
        text+="\n[compression]\nfalse";
    }
    fs::path configFolder = std::string(std::getenv("HOME")) + "/.config/TimeMachine";
    fs::create_directory(configFolder); 
    writeToFile(configFolder / "config.conf", text);
    if(schedule)setupSystemdTimer();
    else{
        system("systemctl --user disable TimeMachine.timer");
    }


}
void SettingsAppWindow::cancelClicked()
{
    std::cout<<"cancel" <<std::endl;
    app->closeWindow("Settings");
}
void SettingsAppWindow::onSelect(Glib::RefPtr<Gio::AsyncResult>& result, Glib::RefPtr<Gtk::FileDialog> &dialog, bool inputDialog){
    auto folder = dialog->select_folder_finish(result);
    std::cout<<folder->get_path()<<std::endl;
    if(inputDialog){
        std::string text = "Input folder: " + folder->get_path();
        m_inputLabel->set_label(text);
        inputPath = folder->get_path();
    }
    else{
        std::string text = "Output folder: " + folder->get_path();
        m_outputLabel->set_label(text);
        outputPath = folder->get_path();
    }

}
void SettingsAppWindow::openFileDialog(bool inputDialog){
    auto dialog = Gtk::FileDialog::create();
    std::string title = "Select folder for ";
    if(inputDialog)title+="files be backed up inside";
    else title+="backups to be stored in";


    dialog->set_title(title);
    dialog->select_folder(*this, 
                            sigc::bind(sigc::mem_fun(*this, &SettingsAppWindow::onSelect),dialog,inputDialog), 
                            NULL);
}



void SettingsAppWindow::setupSystemdTimer(){
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
    system("systemctl --user enable TimeMachine.timer");
}


void SettingsAppWindow::writeToFile(fs::path path, std::string text, bool append){
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


