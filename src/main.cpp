#include <iostream>

#include <gtkmm.h>
#include <glibmm.h>
#include "Pack.h"

#include "SDK.h"
#include "Elements.h"
#include "MainDataBase.h"
#include "ProjectBuilder.h"

using namespace std;
using namespace Gtk;

int main(int argc, char **argv)
{
    Main kit(argc, argv);

    initDirs();

    DEBUG_MSG("open hiasm.db...")
    mdb.open();
    DEBUG_MSG("load CompilerCollection...")
    compilerSet = new CompilerCollection();
    DEBUG_MSG("load PackCollection...")
    packSet = new PackCollection();

    DEBUG_MSG("load scheme...")
    MSDK *sdk = new MSDK();

    ustring filePath = ustring(g_get_current_dir()) + "/HelloWorld.sha";
    cout << "loadFromFile:" << filePath << std::endl;
    cout << "loadFromFileResult:" << sdk->loadFromFile(filePath) << std::endl;

    DEBUG_MSG("build scheme...")
    ProjectBuilder pbuilder;
    pbuilder.build(sdk);


    return 0;
}