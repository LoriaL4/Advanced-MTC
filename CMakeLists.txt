cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
# set the project name
project(advanced_mtc)

# add the executable
add_executable(Example Main.cpp)
#add_executable(Main Test_pool.cpp)
TARGET_LINK_LIBRARIES( Example pthread)
