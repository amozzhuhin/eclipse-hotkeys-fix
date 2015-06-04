project(eclipse-hotkeys-fix C)
cmake_minimum_required(VERSION 2.6)
find_package(PkgConfig REQUIRED)

if(GTK3)
  pkg_check_modules(GDK REQUIRED gdk-3.0)
  pkg_check_modules(GTK REQUIRED gtk+-3.0)
else(GTK3)
  pkg_check_modules(GDK REQUIRED gdk-2.0)
  pkg_check_modules(GTK REQUIRED gtk+-2.0)
endif(GTK3)

set(CMAKE_C_FLAGS -std=c99)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wformat-y2k -Winit-self")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-prototypes -Winline -Wnested-externs -Wbad-function-cast -Wshadow")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DENABLE_DEBUG=1")

# Setup CMake to use GTK
include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})
add_definitions(${GTK_CFLAGS_OTHER})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#
# Main library
#

add_library(eclipse-hotkeys-fix SHARED src/key_events_hook.c)
target_link_libraries(eclipse-hotkeys-fix ${GDK_LIBRARIES})

#
# Test binaries
#

add_executable(gtk-test tests/gtk_test.c)
target_link_libraries(gtk-test ${GTK_LIBRARIES})
