#ifndef BACKUPAPPWINDOW_H_
#define BACKUPAPPWINDOW_H_

#include <gtkmm.h>
#include <filesystem>

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
    void backupSelected();
    void setElementWidths();


protected:
    void size_allocate_vfunc(int width, int height, int baseline) override;
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
    Gtk::ListBox* m_backupList {nullptr};
    Gtk::ListBox* m_fileTree {nullptr};
private:
    void populateBackups(std::filesystem::path backupLocation);
    void populateFileTree(std::filesystem::path fileTreeLocation);

    std::filesystem::path outputDir;

};

#endif
