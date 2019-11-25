cmake_minimum_required(VERSION 3.15)
project(runnable_chain)

set(CMAKE_CXX_STANDARD 14)

add_executable(runnable_chain main.cpp ThreadPool.cpp ThreadPool.h RunnableChain.cpp RunnableChain.h)
add_link_options(pthread)