#ifndef BACKUPAPPLICATION_H
#define BACKUPAPPLICATION_H

#include <gtkmm.h>
#include <vector>
#include <filesystem>
#include <string>
class BackupAppWindow;

class BackupApplication : public Gtk::Application
{
protected:
    BackupApplication();
    
public:
    static Glib::RefPtr<BackupApplication> create();
    void createRecoverWindow(std::vector<std::string> files, std::filesystem::path outputPath);
    void createSettingsWindow();
    void closeWindow(std::string title);
protected: 
    BackupAppWindow* create_appwindow();
    void on_startup() override;
    void on_activate() override;

private:
    void on_action_quit();
    unsigned int recoverWindowID;
};

#endif
