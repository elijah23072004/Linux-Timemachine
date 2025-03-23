#ifndef BACKUPAPPWINDOW_H_
#define BACKUPAPPWINDOW_H_

#include <gtkmm.h>

class BackupAppWindow : public Gtk::ApplicationWindow
{
public:
    BackupAppWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
    
    static BackupAppWindow* create();
    
    void logClicked();
    void backupClicked();
    void restoreClicked();
    void editClicked();
    void settingsClicked();
    void tutorialClicked();
    void quitClicked();

protected:
    Glib::RefPtr<Gtk::Builder> m_refBuilder;
    Gtk::Button* m_log {nullptr};
    Gtk::Button* m_backup {nullptr};
    Gtk::Button* m_restore {nullptr};
    Gtk::Button* m_edit {nullptr};
    Gtk::Button* m_settings {nullptr};
    Gtk::Button* m_tutorial {nullptr};
    Gtk::Button* m_quit {nullptr};
    
    Gtk::SearchBar* m_searchbar {nullptr};
    Gtk::SearchEntry* m_searchentry {nullptr};
    Gtk::ListBox* m_backups {nullptr};
};

#endif
