//Project
#include "SHADialogs.h"

//STL

//Native

//GTKMM

//Qt


SDKDialog shaDlg;

ElementCoreTransmitter *SDKDialog::run(const ustring &name)
{
    for(SDKList::iterator sl = items.begin(); sl != items.end(); sl++)
        if(sl->name == name)
            return sl->e;

    MSDK *sdk = new MSDK();
    ustring f = dataDir + LANG_PATH"en/" + name + ".sha";
    if(file_test(f, FILE_TEST_EXISTS)) {
        sdk->loadFromFile(f);
        for(Element *e : sdk->elements)
            if(e->cIndex == CI_Transmitter) {
                sdk->run(false);
                ElementCoreTransmitter *et = dynamic_cast<ElementCoreTransmitter *>(e);
                items.push_back(SDK_Dialog_Item(name, et));
                return et;
            }
        delete sdk;

    } else {

    }
    return NULL;
}
