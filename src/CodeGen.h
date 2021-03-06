#pragma once
//Project
#include "SDK.h"

//STL

//Native

//GTKMM

//Qt

enum CGT_PARAMS {
    PARAM_CODE_PATH = 0,
    PARAM_DEBUG_MODE,
    PARAM_DEBUG_SERVER_PORT,
    PARAM_DEBUG_CLIENT_PORT,
    PARAM_PROJECT_PATH,
    PARAM_HIASM_VERSION,
    PARAM_USER_NAME,
    PARAM_USER_MAIL,
    PARAM_PROJECT_NAME,
    PARAM_SDE_WIDTH,
    PARAM_SDE_HEIGHT,
    PARAM_COMPILER
} ;


#define CGT_SIZE 85

#define GetCGTools CGT
#define GetCGT_Count CGT_SIZE

extern void *CGT[CGT_SIZE];

class Resources : public std::list<ustring>
{
public:
    int icons;

    void init()
    {
        clear();
        icons = 0;
    }
    inline int nextIcon()
    {
        return icons++;
    }
};

extern Resources resources;
