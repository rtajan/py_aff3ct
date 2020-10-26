cmake_minimum_required(VERSION 3.2)
cmake_policy(SET CMP0054 NEW)

project (my_project)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify bin path
set (EXECUTABLE_OUTPUT_PATH bin/)

# Create the executable from sources
add_executable(my_project ${CMAKE_CURRENT_SOURCE_DIR}/src/main/main.cpp)

# Link with the "Threads library (required to link with AFF3CT after)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Link with AFF3CT
set (AFF3CT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
find_package(AFF3CT CONFIG 2.3.2 REQUIRED)
target_link_libraries(my_project PRIVATE aff3ct::aff3ct-static-lib)

# Link with Python3
find_package (Python3 COMPONENTS Interpreter Development NumPy)
find_package(pybind11 REQUIRED)
target_link_libraries(my_project PRIVATE Python3::NumPy pybind11::embed)

#file(GLOB_RECURSE SRC_FILES_CO ${CMAKE_CURRENT_SOURCE_DIR}/src/common/*)

#add_library          (common STATIC ${SRC_FILES_CO}             )
#target_compile_definitions(common     PRIVATE NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)
target_compile_definitions(my_project PRIVATE NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)
#target_link_libraries(common PRIVATE aff3ct::aff3ct-static-lib)
#target_link_libraries(common PRIVATE Python3::NumPy pybind11::embed)
#target_link_libraries(my_project       PRIVATE common)
#target_include_directories(common      PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/common)
target_include_directories(my_project  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/common)