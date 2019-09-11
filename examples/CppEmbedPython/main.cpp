#include <QApplication>
#include <QDebug>

#include "widget.h"
#include "cpppython.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    CppPython::init();
    QStringList paths;
    paths.append("./");
    CppPython::setModulePath(paths);

    PyObject *demoModule = CppPython::importModule("demo_test");
    if (demoModule)
    {
        PyObject *pFunc = nullptr;
        PyObject *pArg = nullptr;
        PyObject *pyParams = nullptr;

        // load func
        pFunc = PyObject_GetAttrString(demoModule, "hello");
        if (!pFunc) {
            qDebug() << "fun load failed";
        }
        // build params
         pyParams = Py_BuildValue("(s)","barry");
        // call func
        pArg = PyEval_CallObject(pFunc, pyParams);
        // get result
        char *result;
        PyArg_Parse(pArg, "s", &result);
        qDebug() << result;

        PyObject *requestFun = nullptr;
        PyObject *requestArg = nullptr;
        PyObject *requestParams = nullptr;
        requestFun = PyObject_GetAttrString(demoModule, "request");
        if (!pFunc) {
            qDebug() << "fun load failed";
        }
        // call func
        requestArg = PyEval_CallObject(requestFun, requestParams);
        // get result
        char *requestResult;
        PyArg_Parse(requestArg, "s", &requestResult);
        qDebug() << requestResult;
    }
    CppPython::clear();
    return a.exec();
}
