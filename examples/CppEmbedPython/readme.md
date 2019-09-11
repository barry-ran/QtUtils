# C++中嵌入Python脚本
这里是Python嵌入到C++的官方方法[Python官方文档](https://docs.python.org/2/extending/embedding.html)

有几个相关开源库介绍下：

- [PySide2](https://wiki.qt.io/Qt_for_Python)（Qt公司维护）和[PyQt](https://wiki.python.org/moin/PyQt)（小公司维护）都是关于使用python语言借助Qt开发界面程序，与我们需求不一样，不考虑（[两者对比](https://machinekoder.com/pyqt-vs-qt-for-python-pyside2-pyside/)）
- [PythonQt](https://github.com/MeVisLab/pythonqt) - Python嵌入到Qt的优秀开源库，和Qt结合比较好，也可以考虑这个
- [Pybind11](https://github.com/pybind/pybind11) - 将Python和C++绑定的库，方便python和C++通信，既可以用来在使用c++开发python扩展，也可以用来在C++中调用python

下面的例子是使用[Python官方文档](https://docs.python.org/2/extending/embedding.html)中介绍的方法。参考[这个项目](https://github.com/pasimako/embedPython)

## 使用安装包安装python3.7

注意，你程序的位数（32/64）要和python的位数对应。

## Qt项目配置头文件和lib文件
python安装目录/libs/python37.lib复制下，命名为python37_d.lib，debug模式要用
```
win32 {
    # pyhton 路径
    PYTHON_PATH = "D:/Program Files (x86)/python37/"
    # 头文件
    INCLUDEPATH += $${PYTHON_PATH}include
    # 库文件
    LIBS += \
            -L$${PYTHON_PATH}libs -lpython37
}
```

## 代码中执行脚本
- demo_test.py

```
# This Python file uses the following encoding: utf-8

#coding:utf-8
import os

def run(com):
   return com

def main():
    print(run(("4",3)))

if __name__=='__main__':
   main()

```

- cpp代码

注意include "Python.h"放在最前，否则会由于Qt的头文件引出编译问题。

```
// Python.h必须最前面

#include "Python.h"

#include "widget.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    Py_Initialize();
    if (!Py_IsInitialized())
    {
        qDebug() << "init failed";
        Py_Finalize();
        return 0;
    }

    PyObject *pModule;
    PyObject *pFunc = nullptr;
    PyObject *pArg = nullptr;
    PyRun_SimpleString("import sys");
    //设置python模块，搜寻位置，注意qt的工作目录

    PyRun_SimpleString("sys.path.append('./')");
    // load demo
    pModule = PyImport_ImportModule("demo_test");
    if (!pModule) {
        qDebug() << "py load failed";
        Py_Finalize();
        return 0;
    }
    // load func
    pFunc = PyObject_GetAttrString(pModule, "run");
    if (!pFunc) {
        qDebug() << "fun load failed";
        Py_Finalize();
    }
    // build params
    PyObject* pyParams = Py_BuildValue("(s)","helllo world!");
    // call func
    char * result1;
    pArg = PyEval_CallObject(pFunc, pyParams);
    // get result
    PyArg_Parse(pArg, "s", &result1);
    qDebug() << result1;

    return a.exec();
}

```

## 注意脚本文件和python的dll文件要放在程序的工作目录，否则找不到

## API详解

### 运行Python指令
```
PyRun_SimpleString("print(os.getcwd(),a)");
pyext.eval(R"(a+='qwer')");
```

### 加载Python模块
```
PyObject * pModule =PyImport_ImportModule("tp"); //test:Python文件名,若脚本有错则返回空
PyRun_SimpleString("import os");
```

### 给Python的变量赋值

对于数值，使用Py_BuildValue：
```
Py_BuildValue("") None
Py_BuildValue("i", 123) 123
Py_BuildValue("iii", 123, 456, 789) (123, 456, 789)
Py_BuildValue("s", "hello") 'hello'
Py_BuildValue("ss", "hello", "world") ('hello', 'world')
Py_BuildValue("s#", "hello", 4) 'hell'
Py_BuildValue("()") ()
Py_BuildValue("(i)", 123) (123,)  
Py_BuildValue("(ii)", 123, 456) (123, 456)
Py_BuildValue("(i,i)", 123, 456) (123, 456)
Py_BuildValue("[i,i]", 123, 456) [123, 456]
Py_BuildValue("{s:i,s:i}", "abc", 123, "def", 456) {'abc': 123, 'def': 456}
```

对于其他数据结构，使用相应的函数设置，例如：
```
PyObject *pArgs = PyTuple_New(1);
PyObject *pDict = PyDict_New();  //创建字典类型变量 
PyDict_SetItemString(pDict, "Name", Py_BuildValue("s", "WangYao")); //往字典类型变量中填充数据 
PyDict_SetItemString(pDict, "Age", Py_BuildValue("i", 25)); //往字典类型变量中填充数据 
PyTuple_SetItem(pArgs, 0, pDict);//0---序号 将字典类型变量添加到参数元组中 
```

构造好对象以后，通过PyObject_SetAttrString来设置进入Python中:
```
PyObject *ps=PyUnicode_DecodeUTF8(val,strlen(val),"ignore"); //构造了一个对象
PyObject_SetAttrString(p_main_Module,key,ps); //设置
```

### 获取Python变量的值

首先取得变量的指针,然后通过PyArg_Parse解析

```
pModule =PyImport_ImportModule("__main__");
pReturn = PyObject_GetAttrString(pModule, "a"); //可以获得全局变量
int size = PyDict_Size(pReturn); 
PyObject *pNewAge = PyDict_GetItemString(pReturn, "Age"); 
int newAge;
PyArg_Parse(pNewAge, "i", &newAge); 
```

对于元组的解析：
```
int ok;
ok = PyArg_ParseTuple(args, "s", &s); //Python call: f('whoops!')
ok = PyArg_ParseTuple(args, "lls", &k, &l, &s);//Python call: f(1, 2,'three')
ok = PyArg_ParseTuple(args, "(ii)s#", &i, &j, &s, &size);//Python call: f((1, 2), 'three')
ok = PyArg_ParseTuple(args, "s|si", &file, &mode, &bufsize);//Python calls:
//f('spam')
//f('spam', 'w')
//f('spam', 'wb', 100000)
```

### 调用Python函数
```
PyObject * pfun=PyObject_GetAttrString(pModule, "testdict"); //testdict:Python文件中的函数名
PyObject *pReturn = PyEval_CallObject(pfun, pArgs); //调用函数
```

### 设置函数让Python调用

首先定义c函数，然后声明方法列表，然后声明模块，然后增加这个模块，最后调用
```
static int numargs=1890;
static PyObject* emb_numargs(PyObject *self, PyObject *args) //C函数
{
  if(!PyArg_ParseTuple(args, ":numargs"))
    return NULL;
  return PyLong_FromLong(numargs);
}
static PyMethodDef EmbMethods[] = { //方法列表
  {"numargs", emb_numargs, METH_VARARGS,
   "Return the number of arguments received by the process."},
  {NULL, NULL, 0, NULL}
};
static PyModuleDef EmbModule = { //模块声明
  PyModuleDef_HEAD_INIT, "emb", NULL, -1, EmbMethods,
  NULL, NULL, NULL, NULL
};
static PyObject* PyInit_emb(void) //模块初始化函数
{
  return PyModule_Create(&EmbModule);
}
//增加模块：
PyImport_AppendInittab("emb", &PyInit_emb); //增加一个模块
```