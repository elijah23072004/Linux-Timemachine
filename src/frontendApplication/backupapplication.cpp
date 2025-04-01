#include "backupapplication.h"
#include "backupappwindow.h"

#include <iostream>
#include <exception>

BackupApplication::BackupApplication()
: Gtk::Application("com.backupApp.Application", Gio::Application::Flags::HANDLES_OPEN)
{
}

Glib::RefPtr<BackupApplication> BackupApplication::create()
{
    return Glib::make_refptr_for_instance<BackupApplication>(new BackupApplication());
}

BackupAppWindow* BackupApplication::create_appwindow(){
    auto appwindow = BackupAppWindow::create();
    //make sure that the application runs for as long as this window is still open
    add_window(*appwindow);

    appwindow->signal_hide().connect([appwindow](){delete appwindow;});

    return appwindow;
}

void BackupApplication::on_startup()
{
    //call base class's implementation
    Gtk::Application::on_startup();

    add_action("quit", sigc::mem_fun(*this, &BackupApplication::on_action_quit));
    set_accel_for_action("app.quit", "<Ctrl>Q");
}

void BackupApplication::on_activate(){
    try{
        //application is started so show a window
        auto appwindow = create_appwindow();

        appwindow->present();
        appwindow->setElementWidths();
    }
    catch (const Glib::Error& ex)
    {
        std::cerr << "BackupApplication::on_activate(): " <<ex.what() << std::endl;
    }
    catch (const std::exception& ex){
        std::cerr << "BackupApplication::on_activate(): " <<ex.what() << std::endl;
    }
}


void BackupApplication::on_action_quit()
{
    //would call cleanup code for dealing with temportay files nad data
    //however this is not called when force close program like alt+f4 or close button in menu
    std::cout<<"quitting application" <<std::endl;
    auto windows = get_windows();
    for (auto window: windows) window->set_visible(false);
    quit();
}
