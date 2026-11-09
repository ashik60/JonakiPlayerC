#pragma once

#include <string>
#include <vector>
#include <functional>

namespace Jonaki {

/**
 * @brief Independent Command IDs decoupled from proprietary internals.
 */
enum class PlayerAction {
    Play,
    Pause,
    TogglePlayPause,
    Stop,
    SeekForwardSmall,   // 5s
    SeekBackwardSmall,  // 5s
    SeekForwardMedium,  // 30s
    SeekBackwardMedium, // 30s
    SeekForwardLarge,   // 60s
    SeekBackwardLarge,  // 60s
    StepFrameForward,
    StepFrameBackward,
    VolumeUp,
    VolumeDown,
    ToggleMute,
    SpeedUp,
    SpeedDown,
    SpeedReset,
    ToggleFullscreen,
    TogglePlaylist,
    ToggleControlBox,
    OpenPreferences,
    OpenFile,
    OpenUrl,
    ExitApplication
};

struct ActionDefinition {
    PlayerAction action;
    std::string identifier;
    std::string defaultHotkey;
    std::string displayName;
};

class ActionManager {
public:
    static ActionManager& instance();

    const std::vector<ActionDefinition>& getAllActions() const;
    const ActionDefinition* getAction(PlayerAction action) const;
    const ActionDefinition* findByIdentifier(const std::string& id) const;

private:
    ActionManager();
    std::vector<ActionDefinition> m_actions;
};

} // namespace Jonaki
