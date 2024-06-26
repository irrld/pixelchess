cmake_minimum_required (VERSION 3.10.2)

set(GLIST_CXX_STANDARD 20)
set(GLIST_CXX_STANDARD_REQUIRED True)

########## PROJECT NAME ##########
set(APP_NAME PixelChess)


########## USED PLUGINS LIST ##########
if(EMSCRIPTEN)
set(PLUGINS gipWeb)
endif()

##################################################
# DO NOT CHANGE BELOW
##################################################

########## CMAKE THING ##########
if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
endif()


########## ENGINE CONFIGURATION ##########
EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE DEVICEARCHITECTURE )
message( STATUS "Architecture: ${DEVICEARCHITECTURE}" )

set(GLIST_CXX_FLAGS_RELEASE)
set(GLIST_CXX_FLAGS_DEBUG)

# Clang compiler definition for Windows. Do not change this value for other OS.
# For UNIX like systems, configure the compiler via OS commands.
if(WIN32 AND NOT DEFINED EMSCRIPTEN)
    if(NOT DEFINED CMAKE_C_COMPILER)
        set(CMAKE_C_COMPILER "${CMAKE_SOURCE_DIR}/../../zbin/glistzbin-win64/clang64/bin/clang.exe")
    endif()
    if(NOT DEFINED CMAKE_CXX_COMPILER)
        set(CMAKE_CXX_COMPILER "${CMAKE_SOURCE_DIR}/../../zbin/glistzbin-win64/clang64/bin/clang++.exe")
    endif()
endif(WIN32)

if(APPLE AND NOT DEFINED EMSCRIPTEN)
    if(NOT ${DEVICEARCHITECTURE} STREQUAL "arm64")
        set(GLIST_CXX_FLAGS_RELEASE "-march=x86-64")
        set(GLIST_CXX_FLAGS_DEBUG "-march=x86-64")
        set(SYS_NAME "macos-x86_64")
        set(SYS_PATH "/usr/local")
        set(BREW_PATH "${SYS_PATH}/opt")
        set(SYS_INC_PATH "${SYS_PATH}/include")
        set(CMAKE_C_COMPILER "/usr/local/bin/clang")
        set(CMAKE_CXX_COMPILER "/usr/local/bin/clang++")
        set(ENGINE_USE_SYMLINKS ON)
    else()
        set(SYS_NAME "macos")
        set(SYS_PATH "/opt/homebrew")
        set(BREW_PATH "${SYS_PATH}/opt")
        set(SYS_INC_PATH "${SYS_PATH}/include")
        set(CMAKE_C_COMPILER "${BREW_PATH}/llvm/bin/clang")
        set(CMAKE_CXX_COMPILER "${BREW_PATH}/llvm/bin/clang++")
        set(ENGINE_USE_SYMLINKS ON)
    endif()
endif(APPLE)

########## PATHS AND VARIABLES ##########
set(APP_DIR ${CMAKE_SOURCE_DIR})
project (${APP_NAME})
set(CMAKE_BUILD_TYPE Debug)

if(NOT DEFINED TOP_DIR)
    set(TOP_DIR ${APP_DIR}/../..)
endif()
set(MAIN_DIR ${TOP_DIR}/GlistEngine)
set(ENGINE_DIR ${MAIN_DIR}/engine)

if(APPLE)
	add_subdirectory(${ENGINE_DIR} ./_build)
else()
	include(${ENGINE_DIR}/CMakeLists.txt)
endif()

add_subdirectory(znet/znet)


########## SOURCE FILES AND INCLUDE DIRECTORIES ##########

set(GlistApp_SOURCES
        ${APP_DIR}/src/util.cpp
        ${APP_DIR}/src/task.cpp
        ${APP_DIR}/src/color.cpp
        ${APP_DIR}/src/app/main.cpp
        ${APP_DIR}/src/app/app.cpp
        ${APP_DIR}/src/app/button.cpp
        ${APP_DIR}/src/app/textbox.cpp
        ${APP_DIR}/src/app/textbox.cpp
        ${APP_DIR}/src/canvas/promote_screen.cpp
        ${APP_DIR}/src/canvas/loading_canvas.cpp
        ${APP_DIR}/src/canvas/menu_canvas.cpp
        ${APP_DIR}/src/canvas/message_canvas.cpp
        ${APP_DIR}/src/canvas/game_canvas.cpp
        ${APP_DIR}/src/canvas/task_canvas.cpp
        ${APP_DIR}/src/canvas/join_canvas.cpp
        ${APP_DIR}/src/chess/chess_board.cpp
        ${APP_DIR}/src/chess/piece.cpp
        ${APP_DIR}/src/net/packets.cpp
        ${APP_DIR}/src/net/chess_connection.cpp
)


if(WIN32 AND DEBUG)
    add_executable(${APP_NAME} WIN32
            ${GlistApp_SOURCES}
    )
else()
    add_executable(${APP_NAME}
            ${GlistApp_SOURCES}
    )
endif(WIN32 AND DEBUG)

set_target_properties(
    ${APP_NAME}
        PROPERTIES
            CXX_STANDARD ${GLIST_CXX_STANDARD}
            CXX_STANDARD_REQUIRED ${GLIST_CXX_STANDARD_REQUIRED}
)

include_directories(${APP_NAME} PRIVATE
        ${APP_DIR}/src)

########## LINKING ##########
# add lib dependencies
target_link_libraries(${APP_NAME} PRIVATE GlistEngine)
target_link_libraries(${APP_NAME} PRIVATE znet)
