cmake_minimum_required(VERSION 3.24)
project(Project_Maman_14 C)

set(CMAKE_C_STANDARD 90)

add_executable(Project_Maman_14 main.c mainHeader.h preAsm.c preAsm.h funcLib.c assmbler.c assmbler.h)
