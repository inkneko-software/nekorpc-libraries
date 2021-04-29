cmake_minimum_required(VERSION 3.15)
project(nekorpc-libraries)

set(CXX_FLAGS
 -g
 # -DVALGRIND
 # -DMUDUO_STD_STRING
 -Wall
 -Wextra
 # -m32
 -Werror 
 -Wconversion
 -Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 # -MMD
 # -std=c++0x
 -rdynamic
 )
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


add_library(nekorpc_libs_settingreader  ${CMAKE_SOURCE_DIR}/SettingReader/SettingReader.cpp)
add_library(nekorpc_libs_nstring ${CMAKE_SOURCE_DIR}/nstring/webstring.cpp)
target_link_libraries(nekorpc_libs_nstring ssl crypto)

add_subdirectory(tests)

# installation
## header files
install(FILES 
${CMAKE_SOURCE_DIR}/SettingReader/SettingReader.h
${CMAKE_SOURCE_DIR}/nstring/webstring.h
DESTINATION include/nekorpc/libraries/
)
install(TARGETS
nekorpc_libs_settingreader
nekorpc_libs_nstring
LIBRARY DESTINATION lib
)