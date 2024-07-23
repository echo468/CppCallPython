#ifndef PYTHON_CALL_ER_SINGLETON_H
#define PYTHON_CALL_ER_SINGLETON_H

#include <Python.h>
#include <string>
#include <memory>
#include <mutex>

class PythonCallerSingleton {
public:
    static PythonCallerSingleton* getInstance(const std::string &module_name = "demo1",
                                              const std::string &script_path = "D:\\CppCallPython",
                                              const std::string &program_name = "CppCallPython");

    PythonCallerSingleton(const PythonCallerSingleton&) = delete;
    PythonCallerSingleton& operator=(const PythonCallerSingleton&) = delete;

    ~PythonCallerSingleton();

    PyObject* call(const std::string &func_name, PyObject* pArgs = nullptr);

    void switch_module(const std::string &moduleName);

private:
    PythonCallerSingleton(const std::string &module_name,
                          const std::string &script_path,
                          const std::string &program_name);

    PyObject* pModule;

    std::string module_name;

    static int callCount;

    static PythonCallerSingleton* instance;

    static std::mutex mtx;

    static void handle_exception(PyStatus status, PyConfig& config);

    static PyStatus append_to_module_search_paths(PyConfig &config, const std::wstring &path);

    static void show_config(PyConfig &config);
};


#endif
