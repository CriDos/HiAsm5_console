TEMPLATE = app

# Имя файла результата сборки
TARGET = hiasm5_console

# Поддержка стандарта C++14
CONFIG += c++14

# Подключаем Qt
CONFIG += qt
QT += core gui widgets

# Куда копировать результат сборки
DESTDIR += D:/dev/Qt/MainProjects/hiasm5_qt-project_1.0/HiAsm5_build

## *** Подключение gtkmm ***
DEFINES += _USE_MATH_DEFINES
QMAKE_CXXFLAGS += $$system(pkg-config --cflags gtkmm-2.4)
LIBS += -mwindows $$system(pkg-config --libs gtkmm-2.4)
LIBS += -lgthread-2.0 -llibgmodule-2.0

# *** Подключение sqlite-amalgamation ***
# Путь до SQLite
SQLITE_PATH = ../SQLite

LIBS += $$SQLITE_PATH/sqlite3.lib
INCLUDEPATH += $$SQLITE_PATH

# *** Файлы проекта HiAsm 5 ***
HEADERS += \
    src/CodeGen.h \
    src/Compiler.h \
    src/Data.h \
    src/Element.h \
    src/ElementConfig.h \
    src/ElementGrips.h \
    src/ElementPoint.h \
    src/ElementProperty.h \
    src/Elements.h \
    src/MainDataBase.h \
    src/Pack.h \
    src/ProjectBuilder.h \
    src/PropEditor.h \
    src/SDK.h \
    src/SelectManager.h \
    src/SettingsManager.h \
    src/SHADialogs.h \
    src/share.h \
    src/StringList.h

SOURCES += \
    src/CodeGen.cpp \
    src/Compiler.cpp \
    src/Data.cpp \
    src/Element.cpp \
    src/ElementConfig.cpp \
    src/ElementGrips.cpp \
    src/ElementPoint.cpp \
    src/ElementProperty.cpp \
    src/Elements.cpp \
    src/main.cpp \
    src/MainDataBase.cpp \
    src/Pack.cpp \
    src/ProjectBuilder.cpp \
    src/PropEditor.cpp \
    src/SDK.cpp \
    src/SelectManager.cpp \
    src/SettingsManager.cpp \
    src/SHADialogs.cpp \
    src/share.cpp \
    src/StringList.cpp
