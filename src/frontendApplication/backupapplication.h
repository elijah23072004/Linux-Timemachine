#ifndef BACKUPAPPLICATION_H
#define BACKUPAPPLICATION_H

#include <gtkmm.h>

class BackupAppWindow;

class BackupApplication : public Gtk::Application
{
protected:
    BackupApplication();
    
public:
    static Glib::RefPtr<BackupApplication> create();

protected: 
    void on_startup() override;
    void on_activate() override;

private:
    BackupAppWindow* create_appwindow();
    void on_action_quit();
};

#endif
