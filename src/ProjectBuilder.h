#pragma once
//Project
#include "share.h"
#include "SDK.h"

//STL

//Native

//GTKMM

//Qt


class ProjectBuilder
{
private:
    void buildIneternal(MSDK *sdk, const ustring &name);
    void buildWithCodeGen(MSDK *sdk, const ustring &name);

    ustring getOutputProjectName(MSDK *sdk);
public:
    ProjectBuilder();

    /**
     * Build spheme
     * @param sdk pointer to scheme
     */
    void build(MSDK *sdk);
    /**
     * Run project
     * @param sdk pointer to scheme
     */
    void run(MSDK *sdk);
};
