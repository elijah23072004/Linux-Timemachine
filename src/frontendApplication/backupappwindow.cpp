#include "backupappwindow.h"
#include <iostream>
#include <stdexcept>
#include <set>

BackupAppWindow::BackupAppWindow(BaseObjectType* cobject, 
                                 const Glib::RefPtr<Gtk::Builder>& refBuilder)
 :  Gtk::ApplicationWindow(cobject),
    m_refBuilder(refBuilder)
{
    //Get windgets from the Gtk::Builder file
    m_log = m_refBuilder->get_widget<Gtk::Button>("log");
    if(!m_log)
        throw std::runtime_error("No \"log\" object in mainWindow.ui");
    m_backup = m_refBuilder->get_widget<Gtk::Button>("backup");
    if(!m_backup)
        throw std::runtime_error("No \"backup\" object in mainWindow.ui");
    m_restore = m_refBuilder->get_widget<Gtk::Button>("restore");
    if(!m_restore)
        throw std::runtime_error("No \"restore\" object in mainWindow.ui");
    m_edit = m_refBuilder->get_widget<Gtk::Button>("edit");
    if(!m_edit)
        throw std::runtime_error("No \"edit\" object in mainWindow.ui");
    m_settings = m_refBuilder->get_widget<Gtk::Button>("settings");
    if(!m_settings)
        throw std::runtime_error("No \"settings\" object in mainWindow.ui");
    m_tutorial = m_refBuilder->get_widget<Gtk::Button>("tutorial");
    if(!m_tutorial)
       throw std::runtime_error("No \"tutorial\" object in mainWindow.ui");
    
    m_searchentry = m_refBuilder->get_widget<Gtk::SearchEntry>("backupSearchEntry");
    if(!m_searchentry)
        throw std::runtime_error("No \"backupSearchEntry\" object in window.ui");

    m_searchbar = m_refBuilder->get_widget<Gtk::SearchBar>("backupSearch");
    if(!m_searchbar)
        throw std::runtime_error("No \"backupSearch\" object in mainWindow.ui");

    m_backups = m_refBuilder->get_widget<Gtk::ListBox>("backups");
    if(!m_backups)
        throw std::runtime_error("No \"backups\" object in mainWindow.ui");


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


