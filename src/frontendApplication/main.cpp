#include "backupapplication.h"


int main(int argc, char* argv[])
{
    auto application = BackupApplication::create();
    return application->run(argc, argv);
}
