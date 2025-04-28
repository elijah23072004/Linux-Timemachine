#ifndef SETTINGSAPPWINDOW_H
#define SETTINGSAPPWINDOW_H

#include <gtkmm.h>
#include <filesystem>
#include <string>
#include <vector>
#include "backupapplication.h"

namespace fs= std::filesystem;

class SettingsAppWindow : public Gtk::ApplicationWindow
{
public:
    SettingsAppWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
    static SettingsAppWindow* create();

    void saveClicked();
    void cancelClicked();
    void setApplication(BackupApplication* app);
    void openFileDialog(bool inputDialog);
    void onSelect(Glib::RefPtr<Gio::AsyncResult>& result, Glib::RefPtr<Gtk::FileDialog> &dialog, bool inputDialog);
    std::filesystem::path backupLocation;
    std::filesystem::path selectedBackup;
    void writeToFile(fs::path path, std::string text, bool append=false);
    void setupSystemdTimer();
    void autoFillInputs();

protected:
    Glib::RefPtr<Gtk::Builder> m_refBuilder;
    Gtk::Label* m_inputLabel;
    Gtk::Button* m_inputFileDialogButton;   
    Gtk::Label* m_outputLabel;
    Gtk::Button* m_outputFileDialogButton;
    Gtk::CheckButton* m_compression;
    Gtk::CheckButton* m_schedule;
    Gtk::Label* m_advanced;
    Gtk::Text* m_backupRatio;
    Gtk::Button* m_cancel;
    Gtk::Button* m_save;
    Gtk::Label* m_errLabel;
    BackupApplication* app;

    std::string outputPath;
    std::string inputPath;
};

#endif
