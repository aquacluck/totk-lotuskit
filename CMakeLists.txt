cmake_minimum_required(VERSION 3.21)
project(subsdk CXX C ASM)

# Be sure to pass TOTK_VERSION! Version specific builds offer a lot of cpp flexibility (eg versioned structs and symbols)
# for low complexity, so we can do more "static" things without being constexpr+template wizards
add_compile_definitions(TOTK_VERSION=${TOTK_VERSION} TOTK_${TOTK_VERSION}=1)

if (NOT SWITCH)
  # Error if not using switch toolchain file
  message(FATAL_ERROR "Not targeting switch, make sure to specify -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain.cmake")
endif ()

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 26)

# angelscript sources
include_directories(${PROJECT_SOURCE_DIR}/subrepo/angelscript/include)
add_compile_definitions(AS_NO_EXCEPTIONS=1 AS_USE_NAMESPACE=1 AS_64BIT_PTR=1 AS_USE_STLNAMES=1 __SWITCH__=1) # implied: AS_NO_THREADS=1
file(GLOB_RECURSE AS_SOURCES_ASM ${PROJECT_SOURCE_DIR}/subrepo/angelscript/source/*.S)
file(GLOB_RECURSE AS_SOURCES_H ${PROJECT_SOURCE_DIR}/subrepo/angelscript/source/*.h)
file(GLOB_RECURSE AS_SOURCES_CXX ${PROJECT_SOURCE_DIR}/subrepo/angelscript/source/*.cpp)

# exlaunch sources + module config
set(TITLE_ID 0100F2C0115B6000) # TOTK, all versions
set(CONFIG_TITLE_ID 0x${TITLE_ID})
set(CMAKE_EXECUTABLE_SUFFIX ".elf")
add_compile_definitions(EXL_PROGRAM_ID=${CONFIG_TITLE_ID} EXL_LOAD_KIND=Module EXL_LOAD_KIND_ENUM=2)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/module/subsdk9.json.template ${CMAKE_CURRENT_BINARY_DIR}/subsdk9.json)
include_directories(${PROJECT_SOURCE_DIR}/subrepo/exlaunch/source)
include_directories(${PROJECT_SOURCE_DIR}/subrepo/exlaunch/source/lib)
file(GLOB_RECURSE EXL_SOURCES_ASM ${PROJECT_SOURCE_DIR}/subrepo/exlaunch/source/*.s)
file(GLOB_RECURSE EXL_SOURCES_H ${PROJECT_SOURCE_DIR}/subrepo/exlaunch/source/*.h ${PROJECT_SOURCE_DIR}/subrepo/exlaunch/source/*.hpp)
file(GLOB_RECURSE EXL_SOURCES_CXX ${PROJECT_SOURCE_DIR}/subrepo/exlaunch/source/*.cpp ${PROJECT_SOURCE_DIR}/subrepo/exlaunch/source/*.c)

# lotuskit mod sources
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/lib)
include_directories(${PROJECT_SOURCE_DIR}/src/program)
file(GLOB_RECURSE SOURCES_ASM ${PROJECT_SOURCE_DIR}/src/*.s)
file(GLOB_RECURSE SOURCES_H ${PROJECT_SOURCE_DIR}/src/*.h ${PROJECT_SOURCE_DIR}/src/*.hpp)
file(GLOB_RECURSE SOURCES_CXX ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c)

# Include nx tools
include(${CMAKE_SOURCE_DIR}/cmake/SwitchTools.cmake)



# Begin game headers + symbols
include_directories(${PROJECT_SOURCE_DIR}/syms/output/TOTK_${TOTK_VERSION}) # generated output

# Used by header patches to override visibility in specific cases.
# Turn this off and re-clobber the headers for eg private method mangling compatibility (no need for this yet)
add_compile_definitions(LOTUSKIT_HACK_SKIP_PRIVATE=1)

# TODO EventFlow

# nnheaders (NintendoSDK)
add_compile_definitions(
  NNSDK=1
  NN_SDK="15.3.1"
  NN_SDK_MAJOR=15
  NN_SDK_MINOR=3
  NN_SDK_PATCH=1
  NN_SDK_TYPE="Release"
  NN_WARE="1.6.1"
  NN_WARE_MAJOR=1
  NN_WARE_MINOR=6
  NN_WARE_PATCH=1
)
include_directories(${PROJECT_SOURCE_DIR}/subrepo/nnheaders/include) # headers

# agl
include_directories(${PROJECT_SOURCE_DIR}/subrepo/agl/include) # headers

# sead
add_compile_definitions(SEAD_ZSTD=1)
include_directories(${PROJECT_SOURCE_DIR}/subrepo/sead/include) # headers

# zstd
include_directories(${PROJECT_SOURCE_DIR}/subrepo/zstd_include/lib) # headers
#include_directories(${PROJECT_SOURCE_DIR}/subrepo/zstd_include/lib/common)
#include_directories(${PROJECT_SOURCE_DIR}/subrepo/zstd_include/lib/compress)
#include_directories(${PROJECT_SOURCE_DIR}/subrepo/zstd_include/lib/decompress)

# havok

# TODO structured exking header folder (game, engine, ...)
# game, engine, ... -- symbols from the game where I don't have a well-defined header structure are treated as an "exking" lib



# combine sources for subsdk9 build
add_executable(subsdk9 ${EXL_SOURCES_ASM} ${EXL_SOURCES_H} ${EXL_SOURCES_CXX}   ${SOURCES_ASM} ${SOURCES_H} ${SOURCES_CXX}   ${AS_SOURCES_ASM} ${AS_SOURCES_H} ${AS_SOURCES_CXX})
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
