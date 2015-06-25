#pragma once
//Project
#include "share.h"
#include "sqlite3.h"

//STL

//Native

//GTKMM

//Qt

class Compiler
{
private:

public:
    int id;
    ustring name;
    ustring cmd;
    ustring path;
    ustring ext;

    Compiler();
};

class CompilerCollection : public std::list<Compiler *>
{
private:
    void load();
public:
    CompilerCollection();
    Compiler *getById(int id);
};

extern CompilerCollection *compilerSet;
