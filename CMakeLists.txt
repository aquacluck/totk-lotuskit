cmake_minimum_required(VERSION 3.21)
project(subsdk CXX C ASM)

# Be sure to pass TOTK_VERSION! Version specific builds offer a lot of cpp flexibility (eg versioned structs and symbols)
# for low complexity, so we can do more "static" things without being constexpr+template wizards
add_compile_definitions(TOTK_VERSION=${TOTK_VERSION} TOTK_${TOTK_VERSION}=1)

if (NOT SWITCH)
  # Error if not using switch toolchain file
  message(FATAL_ERROR "Not targeting switch, make sure to specify -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain.cmake")
endif ()

set(TITLE_ID 0100F2C0115B6000) # TOTK, all versions
set(CONFIG_TITLE_ID 0x${TITLE_ID})
add_compile_definitions(EXL_PROGRAM_ID=${CONFIG_TITLE_ID} EXL_LOAD_KIND=Module EXL_LOAD_KIND_ENUM=2)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)

# angelscript sources   #add_subdirectory(angelscript)
include_directories(${PROJECT_SOURCE_DIR}/angelscript/include)
add_compile_definitions(AS_NO_EXCEPTIONS=1 __SWITCH__=1)
file(GLOB_RECURSE AS_SOURCES_ASM ${PROJECT_SOURCE_DIR}/angelscript/source/*.S)
file(GLOB_RECURSE AS_SOURCES_H ${PROJECT_SOURCE_DIR}/angelscript/source/*.h)
file(GLOB_RECURSE AS_SOURCES_CXX ${PROJECT_SOURCE_DIR}/angelscript/source/*.cpp)

# exlaunch/subsdk9 mod sources
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/module/subsdk9.json.template ${CMAKE_CURRENT_BINARY_DIR}/subsdk9.json)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/lib)
include_directories(${PROJECT_SOURCE_DIR}/src/program)
file(GLOB_RECURSE SOURCES_ASM ${PROJECT_SOURCE_DIR}/src/*.s)
file(GLOB_RECURSE SOURCES_H ${PROJECT_SOURCE_DIR}/src/*.h ${PROJECT_SOURCE_DIR}/src/*.hpp)
file(GLOB_RECURSE SOURCES_CXX ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c)

# Include nx tools
include(${CMAKE_SOURCE_DIR}/cmake/SwitchTools.cmake)



# Begin game headers + symbols

# Used by header patches to override visibility in specific cases.
# Turn this off and re-clobber the headers for eg private method mangling compatibility (no need for this yet)
add_compile_definitions(LOTUSKIT_HACK_SKIP_PRIVATE=1)

# TODO EventFlow

# NintendoSDK
add_compile_definitions(
  NNSDK=1
  NN_SDK_MAJOR=15
  NN_SDK_MINOR=3
  NN_SDK_PATCH=1
  NN_SDK_TYPE="Release"
  NN_WARE_MAJOR=1
  NN_WARE_MINOR=6
  NN_WARE_PATCH=1
)
include_directories(${PROJECT_SOURCE_DIR}/dt_totk/lib/NintendoSDK/include) # headers
include_directories(${PROJECT_SOURCE_DIR}/syms/output_hpp_sym_ns/TOTK_${TOTK_VERSION}/NintendoSDK) # symbols

# agl
include_directories(${PROJECT_SOURCE_DIR}/dt_totk/lib/agl/include) # headers
include_directories(${PROJECT_SOURCE_DIR}/syms/output_hpp_sym_ns/TOTK_${TOTK_VERSION}/agl) # symbols

# sead
add_compile_definitions(SEAD_ZSTD=1)
include_directories(${PROJECT_SOURCE_DIR}/dt_totk/lib/sead/include) # headers
include_directories(${PROJECT_SOURCE_DIR}/syms/output_hpp_sym_ns/TOTK_${TOTK_VERSION}/sead) # symbols

# zstd
include_directories(${PROJECT_SOURCE_DIR}/dt_totk/lib/zstd/lib) # headers
#include_directories(${PROJECT_SOURCE_DIR}/dt_totk/lib/zstd/lib/common)
#include_directories(${PROJECT_SOURCE_DIR}/dt_totk/lib/zstd/lib/compress)
#include_directories(${PROJECT_SOURCE_DIR}/dt_totk/lib/zstd/lib/decompress)
include_directories(${PROJECT_SOURCE_DIR}/syms/output_hpp_sym_ns/TOTK_${TOTK_VERSION}/zstd) # symbols

# havok
include_directories(${PROJECT_SOURCE_DIR}/syms/output_hpp_sym_ns/TOTK_${TOTK_VERSION}/havok) # symbols

# game, engine, ... -- symbols from the game where I don't have a well-defined header structure (ie not (yet) from dt_totk) are treated as an "exking" lib
# TODO structured exking header folder (game, engine, ...)
include_directories(${PROJECT_SOURCE_DIR}/syms/output_hpp_sym_ns/TOTK_${TOTK_VERSION}/exking) # symbols



# exlaunch/subsdk9 mod flags
set(CMAKE_EXECUTABLE_SUFFIX ".elf")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
#set_source_files_properties(${SOURCES_CXX} PROPERTIES LANGUAGE CXX)
add_executable(subsdk9 ${SOURCES_ASM} ${SOURCES_C} ${SOURCES_H} ${SOURCES_CXX} ${AS_SOURCES_ASM} ${AS_SOURCES_H} ${AS_SOURCES_CXX})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/module/subsdk9.specs.template ${CMAKE_CURRENT_BINARY_DIR}/subsdk9.specs)

# Build nso & generate npdm
add_nso_target_subsdk(subsdk9)
set_target_properties(subsdk9 PROPERTIES CONFIG_JSON "${CMAKE_CURRENT_BINARY_DIR}/subsdk9.json")
__generate_npdm(subsdk9)
set_target_properties(subsdk9 PROPERTIES LINK_FLAGS "-specs ${CMAKE_BINARY_DIR}/subsdk9.specs -g ${ARCH} -nostartfiles")
add_custom_target(subsdk9_meta DEPENDS create_npdm subsdk9_nso)



# Post build
if (FTP_IP)
  ## Upload to target switch
  add_custom_command(TARGET subsdk9_meta POST_BUILD COMMAND ncftpput -u crafty -p boss -P 5000 ${FTP_IP} atmosphere/contents/${TITLE_ID}/exefs ${CMAKE_BINARY_DIR}/main.npdm ${CMAKE_BINARY_DIR}/subsdk9)
endif()

if(RYU_PATH)
  add_custom_command(TARGET subsdk9_meta POST_BUILD COMMAND cp ${CMAKE_BINARY_DIR}/main.npdm ${RYU_PATH}/mods/contents/${TITLE_ID}/mods/exefs/main.npdm)
  add_custom_command(TARGET subsdk9_meta POST_BUILD COMMAND cp ${CMAKE_BINARY_DIR}/subsdk9 ${RYU_PATH}/mods/contents/${TITLE_ID}/mods/exefs/subsdk9)
endif ()
