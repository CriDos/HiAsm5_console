//Project
#include "Pack.h"
#include "SDK.h"
#include "Elements.h"
#include "MainDataBase.h"
#include "ProjectBuilder.h"

//STL
#include <iostream>

//Native

//GTKMM
#include <gtkmm.h>
#include <glibmm.h>

//Qt


using namespace std;
using namespace Gtk;

int main(int argc, char **argv)
{
    Main kit(argc, argv);

    initDirs();

    mdb.open();
    compilerSet = new CompilerCollection();
    packSet = new PackCollection();

    MSDK *sdk = new MSDK();

    ustring filePath = ustring(g_get_current_dir()) + "/test.sha";
    sdk->loadFromFile(filePath);

    ProjectBuilder builder;
    builder.build(sdk);
    builder.run(sdk);

    return 0;
}
