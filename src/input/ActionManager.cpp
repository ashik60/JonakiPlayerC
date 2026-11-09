#include "ActionManager.h"
#include <algorithm>

namespace Jonaki {

ActionManager& ActionManager::instance() {
    static ActionManager s_instance;
    return s_instance;
}

ActionManager::ActionManager() {
    m_actions = {
        {PlayerAction::Play, "playback.play", "", "Play"},
        {PlayerAction::Pause, "playback.pause", "", "Pause"},
        {PlayerAction::TogglePlayPause, "playback.toggle_play_pause", "Space", "Play / Pause"},
        {PlayerAction::Stop, "playback.stop", "Ctrl+Alt+S", "Stop"},
        {PlayerAction::SeekForwardSmall, "playback.seek_forward_5s", "Right", "Seek Forward 5s"},
        {PlayerAction::SeekBackwardSmall, "playback.seek_backward_5s", "Left", "Seek Backward 5s"},
        {PlayerAction::SeekForwardMedium, "playback.seek_forward_30s", "Ctrl+Right", "Seek Forward 30s"},
        {PlayerAction::SeekBackwardMedium, "playback.seek_backward_30s", "Ctrl+Left", "Seek Backward 30s"},
        {PlayerAction::SeekForwardLarge, "playback.seek_forward_60s", "Shift+Right", "Seek Forward 1m"},
        {PlayerAction::SeekBackwardLarge, "playback.seek_backward_60s", "Shift+Left", "Seek Backward 1m"},
        {PlayerAction::StepFrameForward, "playback.step_forward", "F", "Next Frame"},
        {PlayerAction::StepFrameBackward, "playback.step_backward", "D", "Previous Frame"},
        {PlayerAction::VolumeUp, "audio.volume_up", "Up", "Volume Up"},
        {PlayerAction::VolumeDown, "audio.volume_down", "Down", "Volume Down"},
        {PlayerAction::ToggleMute, "audio.mute", "M", "Mute Audio"},
        {PlayerAction::SpeedUp, "playback.speed_up", "C", "Speed Up (0.1x)"},
        {PlayerAction::SpeedDown, "playback.speed_down", "X", "Slow Down (0.1x)"},
        {PlayerAction::SpeedReset, "playback.speed_reset", "Z", "Reset Speed (1.0x)"},
        {PlayerAction::ToggleFullscreen, "window.fullscreen", "Enter", "Toggle Fullscreen"},
        {PlayerAction::TogglePlaylist, "window.playlist", "F8", "Toggle Playlist"},
        {PlayerAction::ToggleControlBox, "window.control_box", "F7", "Toggle Control Box"},
        {PlayerAction::OpenPreferences, "window.preferences", "F5", "Preferences"},
        {PlayerAction::OpenFile, "media.open_file", "Ctrl+O", "Open File..."},
        {PlayerAction::OpenUrl, "media.open_url", "Ctrl+U", "Open URL..."},
        {PlayerAction::ExitApplication, "application.exit", "Alt+F4", "Exit"}
    };
}

const std::vector<ActionDefinition>& ActionManager::getAllActions() const {
    return m_actions;
}

const ActionDefinition* ActionManager::getAction(PlayerAction action) const {
    for (const auto& a : m_actions) {
        if (a.action == action) return &a;
    }
    return nullptr;
}

const ActionDefinition* ActionManager::findByIdentifier(const std::string& id) const {
    for (const auto& a : m_actions) {
        if (a.identifier == id) return &a;
    }
    return nullptr;
}

} // namespace Jonaki
