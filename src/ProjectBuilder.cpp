//Project
#include "ProjectBuilder.h"
#include "CodeGen.h"

//STL

//Native

//GTKMM

//Qt

ProjectBuilder::ProjectBuilder()
{

}

void ProjectBuilder::buildIneternal(MSDK *sdk, const ustring &name)
{
    FILE *f = fopen(name.c_str(), "wb");
    if(!f) {
        return;
    }
    gchar *cont;
    gsize len;
    g_file_get_contents((dataDir + "runner").c_str(), &cont, &len, NULL);
    if(len > 0) {
        fwrite(cont, 1, len, f);
        g_free(cont);
    } else {
        return;
    }
    ustring text;
    sdk->saveToText(text, ustring());
    text = "Make(_base)"LINE_END + text;
    const char *data = text.c_str();
    fwrite(data, strlen(data), 1, f);
    fwrite("\0", 1, 1, f);
    fclose(f);
}

typedef struct {
    void *cgt;
    MSDK *sdk;
    void *result;
} TBuildProcessRec;

typedef int(*TbuildPrepareProc)(void *);
typedef int(*TbuildProcessProc)(TBuildProcessRec *);

void ProjectBuilder::buildWithCodeGen(MSDK *sdk, const ustring &name)
{
    ustring cgen(sdk->pack->path() + "CodeGen.dll");
    if(!file_test(cgen, FILE_TEST_EXISTS)) {
        return;
    }

    GModule *handle  = g_module_open(cgen.c_str(), G_MODULE_BIND_LAZY);
    if(!handle) {
        return;
    }
    TbuildPrepareProc buildPrepareProc;
    if(g_module_symbol(handle, "buildPrepareProc", (gpointer *)&buildPrepareProc)) {
        int x = buildPrepareProc(nullptr);

    } else {

    }

    resources.init();

    TbuildProcessProc buildProcessProc;
    if(g_module_symbol(handle, "buildProcessProc", (gpointer *)&buildProcessProc)) {
        TBuildProcessRec rec;
        rec.cgt = GetCGTools;
        rec.sdk = sdk;
        int x = buildProcessProc(&rec);

        if(sdk->packProject) {
            ustring out_project_name(Glib::filename_display_basename(sdk->fileName));
            out_project_name.erase(out_project_name.length() - 4, 4);   // remove .sha
            ustring out_project_path = sdk->pack->pathCode() + out_project_name;
            ustring out_ext;

            sdk->packProject->make(out_project_path, rec.result);
            Compiler *compiler = sdk->pack->getActiveCompiler();
            if(compiler) {
                ustring cmd = compiler->path + " " + compiler->cmd;

                // replace macros
                const char *upath = "%upath%";
                int i;
                int upathLen = strlen(upath);
                while((i = cmd.find(upath)) > 0) {
                    cmd.replace(i, upathLen, sdk->pack->pathCode());
                }
                const char *opath = "%opath%";
                int opathLen = strlen(opath);
                while((i = cmd.find(opath)) > 0) {
                    cmd.replace(i, opathLen, out_project_path);
                }
                const char *fname = "%fname%";
                int fnameLen = strlen(fname);
                ustring prj(out_project_name);
                prj.append(".").append(sdk->packProject->ext);
                while((i = cmd.find(fname)) > 0) {
                    cmd.replace(i, fnameLen, prj);
                }


                system(cmd.c_str());
                if(!compiler->ext.empty())
                    out_ext = "." + compiler->ext;
            } else {
                if(!sdk->packProject->ext.empty())
                    out_ext = "." + sdk->packProject->ext;
            }
            out_project_path += out_ext;

            GFile *f1 = g_file_new_for_path(out_project_path.c_str());
            GFile *f2 = g_file_new_for_path((name + out_ext).c_str());
            g_file_move(f1, f2, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, NULL);
        }
    }


    //  for(Resources::iterator fn = resources.begin(); fn != resources.end(); fn++)
    //      unlink(fn->c_str());

    g_module_close(handle);
}

ustring ProjectBuilder::getOutputProjectName(MSDK *sdk)
{
    ustring n = sdk->fileName;
    n.erase(n.length() - 4, 4);
    n += ".exe";
    return n;
}

void ProjectBuilder::build(MSDK *sdk)
{

    ustring n = getOutputProjectName(sdk);

    //-------------------------------------------------------------
    if(sdk->pack->name == "_base")
        buildIneternal(sdk, n);
    else {
        buildWithCodeGen(sdk, n);
    }
    //-------------------------------------------------------------


}

void ProjectBuilder::run(MSDK *sdk)
{
    ustring n = getOutputProjectName(sdk);
    if(sdk->pack->name == "_base") {
        //sdk->run();
        system(n.c_str());
    } else {
        sdk->packProject->run(n);
    }
}
