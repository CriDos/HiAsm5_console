#pragma once
//Project

//STL
#include <iostream>

//Native

//GTKMM
#include <gtkmm.h>
#include <glib.h>

//Qt
#include <QtCore>


using namespace Gtk;
using namespace Glib;

#define HIASM_VERSION_MAJOR 5
#define HIASM_VERSION_MINOR 0
#define HIASM_VERSION_BUILD 12
gchar *HIASM_VERSION();

#define LINE_END "\n"
#define PATH_SLASH "\\"

// point types ----------------------------------------------------------------------------------------

#define pt_work  1
#define pt_event 2
#define pt_var   3
#define pt_data  4

// data types -----------------------------------------------------------------------------------------
enum DataType {
    data_null = 0,
    data_int,
    data_str,
    data_data,
    data_combo,
    data_list,
    data_icon,
    data_real,
    data_color,
    data_script,
    data_stream,
    data_bitmap,
    data_wave,
    data_array,
    data_comboEx,
    data_font,
    data_matr,
    data_jpeg,
    data_menu,
    data_code,
    data_element,
    data_flags,
    data_stock,
    data_pixbuf,

    data_count
};

extern const char *dataNames[];

#ifndef max
#define max(x,y) (((x) > (y)) ? (x) : (y))
#define min(x,y) (((x) < (y)) ? (x) : (y))
#endif

// element types ----------------------------------------------------------------------------------
enum ElementType {
    CI_DPElement = 1,
    CI_MultiElement,
    CI_EditMulti,
    CI_EditMultiEx,
    CI_InlineCode,
    CI_DrawElement,
    CI_AS_Special,
    CI_DPLElement,
    CI_UseHiDLL,
    CI_WinElement,
    CI_PointHint,
};

#define str_to_int(s) atoi(s.c_str())
extern ustring int_to_str(int value);
extern ustring double_to_str(double value);

extern char *getTok(char **buf, char tok);

// path ----------------------------------------------------------------------------------------------
#define INT_PATH     "int"PATH_SLASH
#define ICONS_PATH     INT_PATH"icons"PATH_SLASH
#define CURSORS_PATH   INT_PATH"cur"PATH_SLASH
#define DATABASE_FILE  "hiasm.db"
#define LANG_PATH      INT_PATH"lang"PATH_SLASH
#define ELEMENTS_PATH  "elements"PATH_SLASH
#define ELEMENTS_ICON_PATH  "icon"PATH_SLASH
#define ELEMENTS_CODE_PATH  "code"PATH_SLASH
#define ELEMENTS_CONF_PATH  "conf"PATH_SLASH
#define ELEMENTS_NEW_PATH   "new"PATH_SLASH
#define ELEMENTS_MAKE_PATH  "make"PATH_SLASH
#define PACK_ICON_FILE  "icon.png"
#define SPLASH_LOGO_FILE  "splash.png"

#define TABS_STATE_PATH "tabs"PATH_SLASH
#define APP_SETTINGS "settings.ini"

#define ELEMENTS_NIL_FILE ELEMENTS_PATH"_base"PATH_SLASH""ELEMENTS_ICON_PATH"nil.png"

extern ustring databaseFile;
extern ustring dataDir;
extern ustring currentDir;

// internal types --------------------------------------------------------------------------------------
typedef Glib::RefPtr<Gdk::Pixbuf> TypePixbuf;

extern void initDirs();

extern void parseHintName(const ustring &text, ustring &name, ustring &hint);
