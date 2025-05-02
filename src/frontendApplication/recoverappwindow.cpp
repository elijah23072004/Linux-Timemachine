#include "recoverappwindow.h"
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <set>


namespace fs = std::filesystem;

RecoverAppWindow::RecoverAppWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
  : Gtk::ApplicationWindow(cobject),
    m_refBuilder(refBuilder)
{

    auto contentGrid = m_refBuilder->get_widget<Gtk::Grid>("contentGrid");
    if(!contentGrid)
        throw std::runtime_error("No \"backupGrid\" object in recover.ui");
    m_listScrollable = m_refBuilder->get_widget<Gtk::ScrolledWindow>("listScrollable");
    if(!m_listScrollable)
        throw std::runtime_error("No \"listScrollable\" object in recover.ui");
    m_listBox = m_refBuilder->get_widget<Gtk::ListBox>("listBox");
    if(!m_listBox)
        throw std::runtime_error("No \"listBox\" object in recover.ui");
    m_selectAll = m_refBuilder->get_widget<Gtk::Button>("selectAll");
    if(!m_selectAll)
        throw std::runtime_error("No \"selectAll\" object in recover.ui");
    m_selectAll->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &RecoverAppWindow::selectAllClicked)));

    m_outputLabel = m_refBuilder->get_widget<Gtk::Label>("outputLabel");
    if(!m_outputLabel)
        throw std::runtime_error("No \"outputLabel\" object in recover.ui");

    m_fileDialogButton = m_refBuilder->get_widget<Gtk::Button>("fileDialogButton");
    if(!m_fileDialogButton)
        throw std::runtime_error("No \"fileDialogButton\" object in recover.ui");
    m_fileDialogButton->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &RecoverAppWindow::openFileDialog)));
        
    m_cancel = m_refBuilder->get_widget<Gtk::Button>("cancel");
    if(!m_cancel)
        throw std::runtime_error("No \"restore\" object in recover.ui");
    m_cancel->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &RecoverAppWindow::cancelClicked)));

    m_restore = m_refBuilder->get_widget<Gtk::Button>("restore");
    if(!m_restore)
        throw std::runtime_error("No \"restore\" object in recover.ui");

    m_restore->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &RecoverAppWindow::restoreClicked)));
    
    m_errLabel = m_refBuilder->get_widget<Gtk::Label>("errLabel");
    if(!m_errLabel)
        throw std::runtime_error("No \"errLabel\" object in recover.ui");
     
}

RecoverAppWindow* RecoverAppWindow::create(std::vector<std::string> files, fs::path backupLocation)
{
    auto refBuilder = Gtk::Builder::create_from_file("/usr/lib/TimeMachine/recover.ui");
    auto window = Gtk::Builder::get_widget_derived<RecoverAppWindow>(refBuilder, "app_window");
    if (!window) 
        throw std::runtime_error("No \"app_window\" in recover.ui");
    window->populateListBox(files);
    window->backupLocation=backupLocation;
    window->setElementWidths();
    return window;
}



void RecoverAppWindow::populateListBox(std::vector<std::string> files){
    std::cout<<files.size()<<std::endl;
    for(std::string file: files){
        Gtk::CheckButton* button = new Gtk::CheckButton();
        button->set_label(file);
        m_listBox->append(*button);
    }
}

void RecoverAppWindow::setApplication(BackupApplication* app){
    this->app = app;
}
void RecoverAppWindow::restoreClicked()
{
    std::cout<<"restore"<<std::endl;
    if(outputPath == ""){
        std::cout<<"No folder selected"<<std::endl;
        m_errLabel->set_label("Please select an output folder before resoring file");
        return;
    }
    std::vector<std::string> selectedFiles;
   
    int i=0;
    Gtk::ListBoxRow* row;
    while(m_listBox->get_row_at_index(i) != NULL){
        row = m_listBox->get_row_at_index(i);
        i++;
        Gtk::Widget* item = row->get_child();
        Gtk::CheckButton* button = static_cast<Gtk::CheckButton*>(item);
        if(button->get_active() == false){
            continue;
        }
        selectedFiles.push_back(button->get_label());
    }

    

    if(selectedFiles.empty()){
        std::cout<<"No files selected"<<std::endl;
        m_errLabel->set_label("Please select files to restore");
        return;
    }
    selectedBackup = backupLocation.filename();
    backupLocation=backupLocation.parent_path();
    std::string commandStart ="timeMachineCLI -r ";
    std::string commandEnd = " " + backupLocation.string() + " " + outputPath + " " + selectedBackup.string(); 
    for(std::string pathToRecover : selectedFiles){
        if(pathToRecover[pathToRecover.size()-1] =='/'){
            pathToRecover=pathToRecover.substr(0,pathToRecover.size()-1);
        }
        std::string fullCommand = commandStart + pathToRecover + commandEnd;
        std::cout<<fullCommand<<std::endl;
        system(fullCommand.c_str());
    }


    app->closeWindow("Recover Files");
}
void RecoverAppWindow::cancelClicked()
{
    std::cout<<"cancel" <<std::endl;
    app->closeWindow("Recover Files");
}
void RecoverAppWindow::onSelect(Glib::RefPtr<Gio::AsyncResult>& result, Glib::RefPtr<Gtk::FileDialog> &dialog){
    auto folder = dialog->select_folder_finish(result);
    std::cout<<folder->get_path()<<std::endl;
    std::string text = "Output folder: " + folder->get_path();
    m_outputLabel->set_label(text);
    outputPath = folder->get_path();

}
void RecoverAppWindow::openFileDialog(){
    auto dialog = Gtk::FileDialog::create();
    dialog->set_title("Select output folder for files");
    dialog->select_folder(*this, 
                            sigc::bind(sigc::mem_fun(*this, &RecoverAppWindow::onSelect),dialog), 
                            NULL);
}

void RecoverAppWindow::selectAllClicked()
{

    unsigned int i=0;
    Gtk::ListBoxRow* row;
    while(m_listBox->get_row_at_index(i) != NULL){
        row = m_listBox->get_row_at_index(i);
        i++;
        Gtk::Widget* item = row->get_child();
        Gtk::CheckButton* button = static_cast<Gtk::CheckButton*>(item);
        button->set_active(true);
    }
}


void RecoverAppWindow::setElementWidths(){
    int windowWidth= 0;
    windowWidth = this->get_width();
    int windowHeight = this->get_height();
    m_listScrollable->set_size_request(windowWidth*0.66,windowHeight*0.4);
    m_listBox->set_size_request(windowWidth*0.66,-1);

}

void RecoverAppWindow::size_allocate_vfunc(int width, int height, int baseline){
    Gtk::Widget::size_allocate_vfunc(width, height, baseline);
    setElementWidths();
}


