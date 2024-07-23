cmake_minimum_required(VERSION 3.26)
project(CppCallPython)

set(CMAKE_CXX_STANDARD 17)

# Add an executable for the project
add_executable(CppCallPython test.cpp PythonCallerSingleTon.cpp)

# Set Python root directory
set(Python3_ROOT_DIR "D:/pythonenv/python39" CACHE PATH "Path to the Python installation")

# Find Python3 components
find_package(Python3 COMPONENTS Interpreter Development)

# Ensure Python is found
if(Python3_FOUND)
    include_directories(${Python3_INCLUDE_DIRS})
    target_link_libraries(CppCallPython PRIVATE ${Python3_LIBRARIES})
else()
    message(FATAL_ERROR "Python 3.9 not found!")
endif()

# Define Python paths
set(PYTHON_HOME "${Python3_ROOT_DIR}")
set(PYTHON_LIB_DIR "${Python3_ROOT_DIR}/Lib")
set(PYTHON_LIBS_DIR "${Python3_ROOT_DIR}/libs")
set(PYTHON_DLLS_DIR "${Python3_ROOT_DIR}/DLLs")

# Add definitions to be used in the C++ code
add_definitions(
        -DPYTHON_HOME=\"${PYTHON_HOME}\"
        -DPYTHON_LIB_DIR=\"${PYTHON_LIB_DIR}\"
        -DPYTHON_LIBS_DIR=\"${PYTHON_LIBS_DIR}\"
        -DPYTHON_DLLS_DIR=\"${PYTHON_DLLS_DIR}\"
)
