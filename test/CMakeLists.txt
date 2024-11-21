cmake_minimum_required(VERSION 3.5)

project(ctmap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../include
)

add_executable(ctmap main.cpp ../include/ctmap/ctmap.hpp)

include(GNUInstallDirs)
install(TARGETS ctmap
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
