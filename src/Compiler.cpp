﻿//Project
#include "Compiler.h"
#include "MainDataBase.h"

//STL

//Native

//GTKMM

//Qt


CompilerCollection *compilerSet;

Compiler::Compiler()
{

}

CompilerCollection::CompilerCollection()
{
    load();
}

void CompilerCollection::load()
{
    CompilerInfo cmp;

    void *id = mdb.begin_read_compilers();
    while(mdb.read_compilers(id, cmp)) {
        Compiler *compiler = new Compiler();
        compiler->id = cmp.id;
        compiler->ext = cmp.ext;
        compiler->name = cmp.name;
        compiler->path = cmp.path;
        compiler->cmd = cmp.cmd;
        push_back(compiler);
    }
    mdb.end_read(id);
}

Compiler *CompilerCollection::getById(int id)
{
    for(iterator c = begin(); c != end(); ++c)
        if((*c)->id == id)
            return *c;
    return NULL;
}
