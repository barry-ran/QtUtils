#include <QDebug>

#include "cpppython.h"

CppPython::CppPython()
{

}

bool CppPython::init()
{
    Py_Initialize();
    if (!Py_IsInitialized())
    {
        return false;
    }
    PyRun_SimpleString("import sys");
    return true;
}

void CppPython::clear()
{
    if (Py_IsInitialized())
    {
        Py_Finalize();
    }
}

void CppPython::setModulePath(const QStringList &modulePaths)
{
    for (auto &path : modulePaths) {
        PyRun_SimpleString(QString("sys.path.append('%1')").arg(path).toUtf8().data());
    }
}

PyObject *CppPython::importModule(const QString &module)
{
    return PyImport_ImportModule(module.toUtf8().data());
}
