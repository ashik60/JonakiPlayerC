#pragma once

#include <string_view>

namespace Jonaki {

constexpr std::string_view APP_NAME = "JonakiPlayer";
constexpr std::string_view APP_VERSION = "0.1.0";
constexpr std::string_view APP_ORG = "JonakiPlayerTeam";

// PotPlayer default command strings
namespace Commands {
    constexpr std::string_view PLAY_PAUSE = "ID_PLAY_PAUSE";
    constexpr std::string_view STOP = "CMD_MEDIA_CLOSE";
    constexpr std::string_view OPEN_FILE = "ID_BASE_OPEN";
    constexpr std::string_view FULLSCREEN = "ID_FULLSCREEN";
    constexpr std::string_view PLAYLIST = "ID_PLAYLIST_WINDOW";
    constexpr std::string_view CONTROL_BOX = "ID_CONTROL_BOX_WINDOW";
    constexpr std::string_view PREFERENCES = "ID_OPTION_WINDOW";
    constexpr std::string_view APP_EXIT = "ID_APP_EXIT";
}

// Window defaults
namespace UI {
    constexpr int DEFAULT_WINDOW_WIDTH = 960;
    constexpr int DEFAULT_WINDOW_HEIGHT = 540;
    constexpr int MIN_WINDOW_WIDTH = 480;
    constexpr int MIN_WINDOW_HEIGHT = 320;
    constexpr int CONTROL_BAR_HEIGHT = 40;
    constexpr int TITLE_BAR_HEIGHT = 28;
}

} // namespace Jonaki
