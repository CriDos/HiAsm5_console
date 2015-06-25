#pragma once
//STL

//Native

//GTKMM

//Qt

//Project
#include "share.h"
#include "SDK.h"
#include "Elements.h"

typedef struct _SDK_Dialog_Item {
    ustring name;
    ElementCoreTransmitter *e;
    _SDK_Dialog_Item(ustring name, ElementCoreTransmitter *e)
    {
        this->e = e;
        this->name = name;
    }
} SDK_Dialog_Item;

typedef std::list<SDK_Dialog_Item> SDKList;

class SDKDialog
{
public:
    SDKList items;

    ElementCoreTransmitter *run(const ustring &name);
};

extern SDKDialog shaDlg;
