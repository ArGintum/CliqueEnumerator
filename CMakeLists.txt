cmake_minimum_required(VERSION 2.8.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-fconcepts)

FIND_PACKAGE(PythonInterp)

if (PYTHONINTERP_FOUND)
  if (UNIX AND NOT APPLE)
    # Linux
    if (PYTHON_VERSION_MAJOR EQUAL 3)
        FIND_PACKAGE(Boost COMPONENTS python3 numpy3 REQUIRED)
        FIND_PACKAGE(PythonInterp 3)
        FIND_PACKAGE(PythonLibs 3 REQUIRED)
    else()
	message(FATAL_ERROR "Fatal error, building terminated: Python 3 strictly required")
    endif()
  else()
    # MacOS 
    if (PYTHON_VERSION_MAJOR EQUAL 3)
        FIND_PACKAGE(Boost COMPONENTS python3 numpy3 REQUIRED)
        FIND_PACKAGE(PythonInterp 3)
        FIND_PACKAGE(PythonLibs 3 REQUIRED)
    else()
	message(FATAL_ERROR "Fatal error, building terminated: Python 3 strictly required")
    endif()
  endif()
else()
    message(FATAL_ERROR "Python not found")
endif()

message(STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")
message(STATUS "PYTHON_EXECUTABLE = ${PYTHON_EXECUTABLE}")
message(STATUS "PYTHON_INCLUDE_DIRS = ${PYTHON_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}")

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
LINK_LIBRARIES(${Boost_LIBRARIES} ${PYTHON_LIBRARIES}) 

PYTHON_ADD_MODULE(CliqueEnumerator CliqueEnumerator.cpp)
