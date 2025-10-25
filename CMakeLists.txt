cmake_minimum_required(VERSION 3.27)
project(lwork)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

if(MSVC)
    add_compile_options(/MP)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()


#set include directory
include_directories(include)

#set up FetchContent to manage dependencies
#include(FetchContent)
#FetchContent_Declare(
    #    c_types_deps
    #    GIT_REPOSITORY  https://github.com/wujunkai2004/C_types.git
    #    GIT_TAG         main
    #)
#FetchContent_MakeAvailable(c_types_deps)

#read logo file to configure the logo.h
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/src/logo.txt LOGO_TXT_CONTENT)
# escape special characters for C string literal
string(REPLACE "\\" "\\\\" LOGO_TXT_CONTENT "${LOGO_TXT_CONTENT}")
string(REPLACE "\"" "\\\"" LOGO_TXT_CONTENT "${LOGO_TXT_CONTENT}")
string(REPLACE "\n" "\\n\"\\\n\"" LOGO_TXT_CONTENT "${LOGO_TXT_CONTENT}") # 为了美观，将换行符转为 C 的多行字符串
# finally, wrap with double quotes
set(LOGO_CONTENT "${LOGO_TXT_CONTENT}")
# and set the variable for configure_file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/logo.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/logo.h
)

#set source directory
file(GLOB SOURCES "src/*.c")

# add macro END_LINE='\n' to avoid issues with different OS line endings
add_compile_definitions(END_LINE=10)

#add executable with sources and main.c file
add_executable(app ${SOURCES} ${TETRIS} main.c)
target_include_directories(app PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

#add executable for server
#add_executable(server ${SOURCES} ${TETRIS} vendor.c)
#target_link_libraries(server PRIVATE c_types)