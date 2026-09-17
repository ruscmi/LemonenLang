cmake_minimum_required(VERSION 3.15)
project(lmnlang VERSION 2.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto=auto")
find_package(PkgConfig REQUIRED)
find_package(raylib REQUIRED)
find_library(READLINE_LIBRARY NAMES readline)
if(NOT READLINE_LIBRARY)
    message(FATAL_ERROR "libreadline not found! Please install readline development files.")
endif()
add_compile_definitions(NO_FONT_AWESOME)
add_executable(lmnlang
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/interpreter.cpp
    src/manual.cpp
	src/libs/lmngui.cpp
	src/libs/systemlib.cpp
	src/libs/systemlib.cpp
	src/libs/timelib.cpp
	src/libs/libs_initr.cpp
	
)
target_link_libraries(lmnlang PRIVATE ${READLINE_LIBRARY} raylib)
include(GNUInstallDirs)
install(TARGETS lmnlang
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
