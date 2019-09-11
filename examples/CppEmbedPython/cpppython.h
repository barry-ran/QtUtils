#ifndef CPPPYTHON_H
#define CPPPYTHON_H

#include <QStringList>

// https://docs.python.org/3/c-api/init.html

// Python3 defines macro "slots", which is conflict with Qt
#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")

class CppPython
{
public:
    CppPython();

    static bool init();
    static void clear();
    static void setModulePath(const QStringList &modulePaths);
    static PyObject *importModule(const QString &module);
};

#endif // CPPPYTHON_H
