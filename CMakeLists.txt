cmake_minimum_required(VERSION 2.8)

project(scanner)

# bison and flex
find_package(BISON)
find_package(FLEX)
flex_target(SCANNER src/scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp COMPILE_FLAGS -w)
bison_target(PARSER src/parser.y  ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
ADD_FLEX_BISON_DEPENDENCY(SCANNER PARSER)

# src files and make exe
file(GLOB_RECURSE source_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM  source_files ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)


include_directories(includes ${CMAKE_CURRENT_BINARY_DIR} src lib/cereal-1.1.2)

if(USE_DYNAMIC)
	add_library(scanner SHARED ${source_files} 
		${BISON_PARSER_OUTPUTS} ${FLEX_SCANNER_OUTPUTS}) # 使用动态库
else()
	add_library(scanner STATIC ${source_files} 
		${BISON_PARSER_OUTPUTS} ${FLEX_SCANNER_OUTPUTS}) # 使用静态库
endif()

if(NOT BUILD_ALL)

SET (CMAKE_CXX_COMPILER_ENV_VAR "clang++")
SET (CMAKE_CXX_FLAGS "-std=c++11")
SET (CMAKE_CXX_FLAGS_DEBUG "-g")
SET (CMAKE_CXX_FLAGS_MINSIZEREL  "-Os -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE  "-O4 -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(lib/lua_include lib/lex_include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(scannertest
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)
TARGET_LINK_LIBRARIES(scannertest scanner lex lua dl ${GLIBMM_LIBRARIES})

endif()