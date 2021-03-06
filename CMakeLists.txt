# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息
project (Demo1)


# cmake_minimum_required(VERSION 3.4)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
link_directories(${PROJECT_BINARY_DIR}/libs)

#arm-linux-gnueabihf-gcc
#SET(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O2")

# function(include_sub_directories_recursively root_dir)
#   if (IS_DIRECTORY ${root_dir})               # 当前路径是一个目录吗，是的话就加入到包含目录
#     #        if (${root_dir} MATCHES "include")
#     message("include dir: " ${root_dir})
#     include_directories(${root_dir})
#     #        endif()
#   endif()

#   file(GLOB ALL_SUB RELATIVE ${root_dir} ${root_dir}/*) # 获得当前目录下的所有文件，让如ALL_SUB列表中
#   foreach(sub ${ALL_SUB})
#     if (IS_DIRECTORY ${root_dir}/${sub})
#       include_sub_directories_recursively(${root_dir}/${sub}) # 对子目录递归调用，包含
#     endif()
#   endforeach()
# endfunction()

MACRO(SUBDIRLIST result curdir)

  #  FILE(GLOB_RECURSE children LIST_DIRECTORIES true  RELATIVE ${curdir} CONFIGURE_DEPENDS ${curdir}/*)
  #FILE(GLOB_RECURSE children ${curdir}/*)
  file(GLOB_RECURSE children LIST_DIRECTORIES true ${curdir}/*)
  #message(+-+-+-+-${children})
  SET(dirlist ${CMAKE_CURRENT_SOURCE_DIR}/${curdir})
  #message(top_dir++++++${dirlist})

  FOREACH(child ${children})
    #message(++++${child})
    
    IF(IS_DIRECTORY ${child})
      LIST(APPEND dirlist ${child})
      #message(++++${dirlist})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

# 设定源码根目录
set(ROOT_DIR ".")
message(STATUS "ROOT_DIR = " ${ROOT_DIR})

# 查找源码目录
SUBDIRLIST(SUBDIRS ${ROOT_DIR})


# 添加源文件
FOREACH(SRC_PATH ${SUBDIRS})
  message(---------- ${SRC_PATH})
#  aux_source_directory(${SRC_PATH} DIR_SRCS)
ENDFOREACH(SRC_PATH)

aux_source_directory(. DIR_SRCS)
add_subdirectory(math)
add_subdirectory(log)
add_subdirectory(io)
#add_subdirectory(log)

link_libraries(math log)
link_libraries(pthread)
link_libraries(io)

# 指定生成目标
add_executable(test ${DIR_SRCS})
#target_link_libraries(test, math)

message(STATUS "CMAKE_C_COMPILER = " ${CMAKE_C_COMPILER})
message(STATUS "CMAKE_CXX_COMPILER = " ${CMAKE_CXX_COMPILER})
#message(STATUS "CMAKE_C_FLAGS_DEBUG = " ${CMAKE_C_FLAGS_DEBUG})

message(STATUS "CMAKE_C_FLAGS = " ${CMAKE_C_FLAGS})
message(STATUS "CMAKE_C_FLAGS_DEBUG = " ${CMAKE_C_FLAGS_DEBUG})
message(STATUS "CMAKE_C_FLAGS_MINSIZEREL = " ${CMAKE_C_FLAGS_MINSIZEREL})
message(STATUS "CMAKE_C_FLAGS_RELEASE = " ${CMAKE_C_FLAGS_RELEASE})
message(STATUS "CMAKE_C_FLAGS_RELWITHDEBINFO = " ${CMAKE_C_FLAGS_RELWITHDEBINFO})

message(STATUS "CMAKE_CXX_FLAGS = " ${CMAKE_CXX_FLAGS})
message(STATUS "CMAKE_CXX_FLAGS_DEBUG = " ${CMAKE_CXX_FLAGS_DEBUG})
message(STATUS "CMAKE_CXX_FLAGS_MINSIZEREL = " ${CMAKE_CXX_FLAGS_MINSIZEREL})
message(STATUS "CMAKE_CXX_FLAGS_RELEASE = " ${CMAKE_CXX_FLAGS_RELEASE})
message(STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO = " ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})

message(STATUS "CMAKE_EXE_LINKER_FLAGS = " ${CMAKE_EXE_LINKER_FLAGS})
message(STATUS "CMAKE_EXE_LINKER_FLAGS_DEBUG = " ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
message(STATUS "CMAKE_EXE_LINKER_FLAGS_MINSIZEREL = " ${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL})
message(STATUS "CMAKE_EXE_LINKER_FLAGS_RELEASE = " ${CMAKE_EXE_LINKER_FLAGS_RELEASE})
message(STATUS "CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO = " ${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO})

message(STATUS "CMAKE_MODULE_LINKER_FLAGS = " ${CMAKE_MODULE_LINKER_FLAGS})
message(STATUS "CMAKE_MODULE_LINKER_FLAGS_DEBUG = " ${CMAKE_MODULE_LINKER_FLAGS_DEBUG})
message(STATUS "CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL = " ${CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL})
message(STATUS "CMAKE_MODULE_LINKER_FLAGS_RELEASE = " ${CMAKE_MODULE_LINKER_FLAGS_RELEASE})
message(STATUS "CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO = " ${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO})

message(STATUS "CMAKE_SHARED_LINKER_FLAGS = " ${CMAKE_SHARED_LINKER_FLAGS})
message(STATUS "CMAKE_SHARED_LINKER_FLAGS_DEBUG = " ${CMAKE_SHARED_LINKER_FLAGS_DEBUG})
message(STATUS "CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL = " ${CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL})
message(STATUS "CMAKE_SHARED_LINKER_FLAGS_RELEASE = " ${CMAKE_SHARED_LINKER_FLAGS_RELEASE})
message(STATUS "CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO = " ${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO})

message(STATUS "CMAKE_STATIC_LINKER_FLAGS = " ${CMAKE_STATIC_LINKER_FLAGS})
message(STATUS "CMAKE_STATIC_LINKER_FLAGS_DEBUG = " ${CMAKE_STATIC_LINKER_FLAGS_DEBUG})
message(STATUS "CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL = " ${CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL})
message(STATUS "CMAKE_STATIC_LINKER_FLAGS_RELEASE = " ${CMAKE_STATIC_LINKER_FLAGS_RELEASE})
message(STATUS "CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO = " ${CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO})

message("PROJECT_SOURCE_DIR=" ${PROJECT_SOURCE_DIR})

message("PROJECT_BINARY_DIR=" ${PROJECT_BINARY_DIR})

message("CMAKE_CURRENT_LIST_LINE= " ${CMAKE_CURRENT_LIST_LINE})
message("CMAKE_CURRENT_LIST_FILE= " ${CMAKE_CURRENT_LIST_FILE})

message("LIBRARY_OUTPUT_PATH= " ${LIBRARY_OUTPUT_PATH})