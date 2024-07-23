#include "PythonCallerSingleton.h"
#include <iostream>
#include <vector>

int PythonCallerSingleton::callCount = 0;
PythonCallerSingleton* PythonCallerSingleton::instance = nullptr;
std::mutex PythonCallerSingleton::mtx;

PythonCallerSingleton* PythonCallerSingleton::getInstance(const std::string &module_name,
                                                          const std::string &script_path,
                                                          const std::string &program_name) {
    std::unique_lock<std::mutex> lock(mtx);
    if (instance == nullptr) {
        instance = new PythonCallerSingleton(module_name, script_path, program_name);
    } else if (instance->module_name != module_name) {
        instance->module_name = module_name;
        instance->switch_module(module_name);
    }
    return instance;
}

PythonCallerSingleton::PythonCallerSingleton(const std::string &module_name,
                                             const std::string &script_path,
                                             const std::string &program_name) {
    PyPreConfig preConfig;
    PyConfig config;

    PyPreConfig_InitIsolatedConfig(&preConfig);
    PyConfig_InitIsolatedConfig(&config);

    wchar_t* wide_program_name = Py_DecodeLocale(program_name.c_str(), nullptr);

    PyStatus status = PyConfig_SetString(&config, &config.program_name, wide_program_name);
    if (PyStatus_Exception(status)) {
        handle_exception(status, config);
        return;
    }

    status = PyConfig_SetString(&config, &config.home, L"" PYTHON_HOME "");
    if (PyStatus_Exception(status)) {
        handle_exception(status, config);
        return;
    }

    config.module_search_paths_set = 1;

    std::vector<std::wstring> paths = {
            L"" PYTHON_LIB_DIR "",
            L"" PYTHON_LIBS_DIR "",
            L"" PYTHON_DLLS_DIR "",
    };

    for (const auto& path : paths) {
        status = append_to_module_search_paths(config, path);
        if (PyStatus_Exception(status)) {
            handle_exception(status, config);
            return;
        }
    }

    wchar_t* wide_script_path = Py_DecodeLocale(script_path.c_str(), nullptr);

    status = append_to_module_search_paths(config, wide_script_path);
    if (PyStatus_Exception(status)) {
        handle_exception(status, config);
        return;
    }

    if (!Py_IsInitialized()) {
        status = Py_InitializeFromConfig(&config);
        if (PyStatus_Exception(status)) {
            handle_exception(status, config);
            return;
        }
    }

    PyMem_RawFree(wide_program_name);
    PyMem_RawFree(wide_script_path);

    switch_module(module_name);
}

PythonCallerSingleton::~PythonCallerSingleton() {
    std::cout << "PythonCaller Destructor" << std::endl;
    Py_XDECREF(pModule);
    if (Py_FinalizeEx() < 0) {
        std::cerr << "Failed to finalize Python interpreter." << std::endl;
        PyErr_Print();
        return;
    }
    delete instance;
}

PyObject* PythonCallerSingleton::call(const std::string &func_name, PyObject* pArgs) {
    PyObject *pFunc = PyObject_GetAttrString(pModule, func_name.c_str());
    if (!pFunc) {
        std::cerr << "Failed to import func: " << func_name << std::endl;
        PyErr_Print();
        return nullptr;
    }

    PyObject *pReturn = PyObject_CallObject(pFunc, pArgs);
    Py_XDECREF(pFunc);

    if (!pReturn) {
        std::cerr << "Failed to call func: " << func_name << std::endl;
        PyErr_Print();
        return nullptr;
    }

    return pReturn;
}

void PythonCallerSingleton::switch_module(const std::string &moduleName) {
    pModule = PyImport_ImportModule(moduleName.c_str());
    if (!pModule) {
        std::cerr << "Failed to switch module: " << moduleName << std::endl;
        PyErr_Print();
        return;
    }
}

void PythonCallerSingleton::handle_exception(PyStatus status, PyConfig& config) {
    PyConfig_Clear(&config);
    Py_ExitStatusException(status);
}

PyStatus PythonCallerSingleton::append_to_module_search_paths(PyConfig &config, const std::wstring &path) {
    return PyWideStringList_Append(&config.module_search_paths, path.c_str());
}

void PythonCallerSingleton::show_config(PyConfig &config) {
    printf("Python version: %s\n", Py_GetVersion());
    printf("[Info]Program name: %ls\n", config.program_name);
    printf("[Info]Home directory: %ls\n", config.home);
    printf("[Info]Module search paths:\n");
    for (Py_ssize_t i = 0; i < config.module_search_paths.length; ++i) {
        printf("\t%ls\n", config.module_search_paths.items[i]);
    }
}
