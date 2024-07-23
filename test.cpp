#include <iostream>
#include <mutex>
#include "PythonCallerSingleTon.h"
#include <random>


void callDemo1() {
    // Test for demo1.py
    PythonCallerSingleton* caller = PythonCallerSingleton::getInstance("demo1");
    std::cout << "demo1 caller address: " << caller << std::endl;
    PyObject * result1 = caller->call("hello_world");
    if (result1 != nullptr) {
        const char *result_chars = PyUnicode_AsUTF8(result1);
        std::cout << result_chars << std::endl;
        Py_DECREF(result1);
    }
}

void callDemo2() {
    // Test for demo2.py
    PythonCallerSingleton* caller = PythonCallerSingleton::getInstance("demo2");
    std::cout << "demo2 caller address: " << caller << std::endl;
    PyObject * pArgs = PyTuple_New(2);
    PyTuple_SetItem(pArgs, 0, PyLong_FromLong(rand() % 100));
    PyTuple_SetItem(pArgs, 1, PyLong_FromLong(rand() % 100));
    PyObject * result2 = caller->call("add", pArgs);
    if (result2 != nullptr) {
        long sum = PyLong_AsLong(result2);
        std::cout << "The sum is: " << sum << std::endl;
        Py_DECREF(result2);
    }
    Py_DECREF(pArgs);
}

void callDemo3() {
    // Test for demo3.py
    PythonCallerSingleton* caller = PythonCallerSingleton::getInstance("demo3");
    std::cout << "demo3 caller address: " << caller << std::endl;
    PyObject * pList = PyList_New(5);
    for (int i = 0; i < 5; ++i) {
        PyList_SetItem(pList, i, PyFloat_FromDouble(i + 1));
    }
    PyObject * nArgs = PyTuple_New(1);
    PyTuple_SetItem(nArgs, 0, pList);
    PyObject * result3 = caller->call("calculate_mean", nArgs);
    if (result3 != nullptr) {
        double mean = PyFloat_AsDouble(result3);
        std::cout << "The mean is: " << mean << std::endl;
        Py_DECREF(result3);
    }
    Py_DECREF(nArgs);
}

std::mutex mtx;

void SingletonTest() {
    std::unique_lock<std::mutex> lock(mtx);
    callDemo1();
    callDemo2();
    callDemo3();
}



int main() {
    SingletonTest();
    return 0;
}
