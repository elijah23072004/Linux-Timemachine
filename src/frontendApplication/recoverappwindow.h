#ifndef RECOVERAPPWINDOW_H
#define RECOVERAPPWINDOW_H

#include <gtkmm.h>
#include <filesystem>
#include <string>
#include <vector>
#include "backupapplication.h"
class RecoverAppWindow : public Gtk::ApplicationWindow
{
public:
    RecoverAppWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
    static RecoverAppWindow* create(std::vector<std::string> files, std::filesystem::path backupLocation);
    void setElementWidths();

    void restoreClicked();
    void cancelClicked();
    void selectAllClicked();
    void setApplication(BackupApplication* app);
    void openFileDialog();
    void onSelect(Glib::RefPtr<Gio::AsyncResult>& result, Glib::RefPtr<Gtk::FileDialog> &dialog);
    void populateListBox(std::vector<std::string> files);
    std::filesystem::path backupLocation;
    std::filesystem::path selectedBackup;
protected:
    void size_allocate_vfunc(int width, int height, int baseline) override;

    Glib::RefPtr<Gtk::Builder> m_refBuilder;
    Gtk::ScrolledWindow* m_listScrollable;
    Gtk::ListBox* m_listBox;
    Gtk::Button* m_fileDialogButton;
    Gtk::Button* m_cancel;
    Gtk::Button* m_restore;
    Gtk::Label* m_outputLabel;
    Gtk::Label* m_errLabel;
    Gtk::Button* m_selectAll;
    BackupApplication* app;
    std::string outputPath;
    std::filesystem::path backupSelected;    
};

#endif
