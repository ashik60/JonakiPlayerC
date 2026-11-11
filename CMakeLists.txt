cmake_minimum_required(VERSION 3.24)

project(JonakiPlayer
    VERSION 0.1.0
    DESCRIPTION "Autonomous Cross-Platform PotPlayer Reimplementation"
    LANGUAGES C CXX
)

# Enforce C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Qt 6 Configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    OpenGL
    OpenGLWidgets
)

# PkgConfig & libmpv
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(MPV QUIET mpv)
    pkg_check_modules(SQLITE3 QUIET sqlite3)
endif()

if(NOT SQLITE3_FOUND)
    find_path(SQLITE3_INCLUDE_DIRS NAMES sqlite3.h)
    find_library(SQLITE3_LIBRARIES NAMES sqlite3)
endif()

if(NOT MPV_FOUND)
    find_path(MPV_INCLUDE_DIRS NAMES mpv/client.h)
    find_library(MPV_LIBRARIES NAMES mpv)
    if(MPV_INCLUDE_DIRS AND MPV_LIBRARIES)
        set(MPV_FOUND TRUE)
    endif()
endif()

if(MPV_FOUND)
    message(STATUS "Found libmpv: ${MPV_LIBRARIES}")
else()
    message(FATAL_ERROR "libmpv required for Milestone M2 playback engine!")
endif()

# Compiler warnings and optimization flags
if(MSVC)
    add_compile_options(/W4 /permissive- /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${MPV_INCLUDE_DIRS}
)

# Source file definitions
set(CORE_SOURCES
    src/core/Logger.cpp
    src/core/Logger.h
    src/core/Constants.h
    src/core/EventBus.h
)

set(SETTINGS_SOURCES
    src/settings/SettingsManager.cpp
    src/settings/SettingsManager.h
    src/settings/SettingsTransaction.cpp
    src/settings/SettingsTransaction.h
)

set(INPUT_SOURCES
    src/input/ActionManager.cpp
    src/input/ActionManager.h
)

set(PLAYBACK_SOURCES
    src/playback/IPlaybackEngine.h
    src/playback/MpvPlaybackEngine.cpp
    src/playback/MpvPlaybackEngine.h
)

set(VIDEO_SOURCES
    src/video/VideoOpenGLWidget.cpp
    src/video/VideoOpenGLWidget.h
)

set(PLAYLIST_SOURCES
    src/playlist/DplPlaylistParser.cpp
    src/playlist/DplPlaylistParser.h
    src/playlist/ResumeStore.cpp
    src/playlist/ResumeStore.h
    src/playlist/PlaylistModel.cpp
    src/playlist/PlaylistModel.h
    src/playlist/PlaylistController.cpp
    src/playlist/PlaylistController.h
)

set(SUBTITLE_SOURCES
    src/subtitles/SubtitleManager.cpp
    src/subtitles/SubtitleManager.h
)

set(UI_SOURCES
    src/ui/MainWindow.cpp
    src/ui/MainWindow.h
    src/ui/SkinRenderer.cpp
    src/ui/SkinRenderer.h
    src/ui/SkinParser.cpp
    src/ui/SkinParser.h
    src/ui/FramelessWindowHelper.cpp
    src/ui/FramelessWindowHelper.h
    src/ui/TitleBarWidget.cpp
    src/ui/TitleBarWidget.h
    src/ui/BottomControlBar.cpp
    src/ui/BottomControlBar.h
    src/ui/PlaylistDrawer.cpp
    src/ui/PlaylistDrawer.h
    src/ui/ContextMenuBuilder.cpp
    src/ui/ContextMenuBuilder.h
    src/ui/FrameCaptureHelper.cpp
    src/ui/FrameCaptureHelper.h
    src/ui/PreferencesDialog.cpp
    src/ui/PreferencesDialog.h
    src/ui/preferences/PreferencesPageBase.cpp
    src/ui/preferences/PreferencesPageBase.h
    src/ui/preferences/GeneralPreferencesPage.cpp
    src/ui/preferences/GeneralPreferencesPage.h
    src/ui/preferences/PlaybackPreferencesPage.cpp
    src/ui/preferences/PlaybackPreferencesPage.h
    src/ui/preferences/VideoPreferencesPage.cpp
    src/ui/preferences/VideoPreferencesPage.h
    src/ui/preferences/AudioPreferencesPage.cpp
    src/ui/preferences/AudioPreferencesPage.h
    src/ui/preferences/SubtitlePreferencesPage.cpp
    src/ui/preferences/SubtitlePreferencesPage.h
    src/ui/preferences/FilterControlPreferencesPage.cpp
    src/ui/preferences/FilterControlPreferencesPage.h
)

set(APP_SOURCES
    src/application/main.cpp
    src/application/JonakiApplication.cpp
    src/application/JonakiApplication.h
)

# Main Executable
add_executable(JonakiPlayer
    ${APP_SOURCES}
    ${CORE_SOURCES}
    ${SETTINGS_SOURCES}
    ${INPUT_SOURCES}
    ${PLAYBACK_SOURCES}
    ${PLAYLIST_SOURCES}
    ${SUBTITLE_SOURCES}
    ${VIDEO_SOURCES}
    ${UI_SOURCES}
)

target_link_libraries(JonakiPlayer PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    ${MPV_LIBRARIES}
    sqlite3
)

target_compile_definitions(JonakiPlayer PRIVATE HAVE_MPV=1)

# Platform specific configurations
if(WIN32)
    target_compile_definitions(JonakiPlayer PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(JonakiPlayer PRIVATE dwmapi shell32 user32 opengl32)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(JonakiPlayer PRIVATE pthread GL)
endif()

# Enable Testing
enable_testing()
add_subdirectory(tests)
