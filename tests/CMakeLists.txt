# Tests CMakeLists.txt

find_package(Qt6 REQUIRED COMPONENTS Test Core)

# 1. Settings Unit Tests
add_executable(SettingsTest
    unit/test_settings.cpp
    ../src/settings/SettingsManager.cpp
    ../src/core/Logger.cpp
)

target_link_libraries(SettingsTest PRIVATE
    Qt6::Core
    Qt6::Test
)

add_test(NAME SettingsTest COMMAND SettingsTest)

# 2. Mock Playback Engine Unit Tests
add_executable(PlaybackEngineMockTest
    unit/test_playback_mock.cpp
    ../src/core/Logger.cpp
)

target_link_libraries(PlaybackEngineMockTest PRIVATE
    Qt6::Core
    Qt6::Test
)

add_test(NAME PlaybackEngineMockTest COMMAND PlaybackEngineMockTest)

# 3. Real libmpv Integration Tests
add_executable(MpvPlaybackIntegrationTest
    integration/test_mpv_integration.cpp
    ../src/playback/MpvPlaybackEngine.cpp
    ../src/core/Logger.cpp
)

target_link_libraries(MpvPlaybackIntegrationTest PRIVATE
    Qt6::Core
    Qt6::Test
    ${MPV_LIBRARIES}
)

target_compile_definitions(MpvPlaybackIntegrationTest PRIVATE HAVE_MPV=1)

if(WIN32)
    target_compile_definitions(MpvPlaybackIntegrationTest PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(MpvPlaybackIntegrationTest PRIVATE dwmapi shell32 user32)
endif()

add_test(NAME MpvPlaybackIntegrationTest COMMAND MpvPlaybackIntegrationTest)

# 4. Skin Parser Unit Tests
add_executable(SkinParserTest
    unit/test_skin_parser.cpp
    ../src/ui/SkinParser.cpp
    ../src/core/Logger.cpp
)

target_link_libraries(SkinParserTest PRIVATE
    Qt6::Core
    Qt6::Test
)

add_test(NAME SkinParserTest COMMAND SkinParserTest)

# 5. Playlist & Resume Store Unit Tests
add_executable(PlaylistTest
    unit/test_playlist.cpp
    ../src/playlist/DplPlaylistParser.cpp
    ../src/playlist/ResumeStore.cpp
    ../src/playlist/PlaylistModel.cpp
    ../src/core/Logger.cpp
)

target_link_libraries(PlaylistTest PRIVATE
    Qt6::Core
    Qt6::Test
    sqlite3
)

add_test(NAME PlaylistTest COMMAND PlaylistTest)

# 6. Subtitles & OSD Unit Tests
add_executable(SubtitleTest
    unit/test_subtitles.cpp
    ../src/subtitles/SubtitleManager.cpp
    ../src/playback/MpvPlaybackEngine.cpp
    ../src/core/Logger.cpp
)

target_link_libraries(SubtitleTest PRIVATE
    Qt6::Core
    Qt6::Test
    ${MPV_LIBRARIES}
)

target_compile_definitions(SubtitleTest PRIVATE HAVE_MPV=1)

if(WIN32)
    target_compile_definitions(SubtitleTest PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(SubtitleTest PRIVATE dwmapi shell32 user32)
endif()

add_test(NAME SubtitleTest COMMAND SubtitleTest)

# 7. Audio & Video Filters Unit/Integration Tests
add_executable(AudioVideoFilterTest
    unit/test_audio_video_filters.cpp
    ../src/playback/MpvPlaybackEngine.cpp
    ../src/settings/SettingsManager.cpp
    ../src/core/Logger.cpp
)

target_link_libraries(AudioVideoFilterTest PRIVATE
    Qt6::Core
    Qt6::Test
    ${MPV_LIBRARIES}
)

target_compile_definitions(AudioVideoFilterTest PRIVATE HAVE_MPV=1)

if(WIN32)
    target_compile_definitions(AudioVideoFilterTest PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(AudioVideoFilterTest PRIVATE dwmapi shell32 user32)
endif()

add_test(NAME AudioVideoFilterTest COMMAND AudioVideoFilterTest)

# 9. Preferences Dialog & Settings Transaction Unit Tests
add_executable(PreferencesDialogTest
    unit/test_preferences_dialog.cpp
    ../src/settings/SettingsManager.cpp
    ../src/settings/SettingsTransaction.cpp
    ../src/ui/PreferencesDialog.cpp
    ../src/ui/preferences/PreferencesPageBase.cpp
    ../src/ui/preferences/GeneralPreferencesPage.cpp
    ../src/ui/preferences/PlaybackPreferencesPage.cpp
    ../src/ui/preferences/VideoPreferencesPage.cpp
    ../src/ui/preferences/AudioPreferencesPage.cpp
    ../src/ui/preferences/SubtitlePreferencesPage.cpp
    ../src/ui/preferences/FilterControlPreferencesPage.cpp
    ../src/core/Logger.cpp
)

target_link_libraries(PreferencesDialogTest PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Test
)

add_test(NAME PreferencesDialogTest COMMAND PreferencesDialogTest)

# 10. Frame Capture Subsystem Integration Tests
add_executable(FrameCaptureTest
    integration/test_frame_capture.cpp
    ../src/playback/MpvPlaybackEngine.cpp
    ../src/ui/FrameCaptureHelper.cpp
    ../src/settings/SettingsManager.cpp
    ../src/core/Logger.cpp
)

target_link_libraries(FrameCaptureTest PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Test
    ${MPV_LIBRARIES}
)

target_compile_definitions(FrameCaptureTest PRIVATE HAVE_MPV=1)

if(WIN32)
    target_compile_definitions(FrameCaptureTest PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(FrameCaptureTest PRIVATE dwmapi shell32 user32)
endif()

add_test(NAME FrameCaptureTest COMMAND FrameCaptureTest)

# 11. Open URL Dialog Unit Tests
add_executable(OpenUrlDialogTest
    unit/test_open_url_dialog.cpp
    ../src/ui/OpenUrlDialog.cpp
    ../src/network/UrlValidator.cpp
    ../src/settings/SettingsManager.cpp
    ../src/core/Logger.cpp
)

target_link_libraries(OpenUrlDialogTest PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Test
)

add_test(NAME OpenUrlDialogTest COMMAND OpenUrlDialogTest)

# 12. Network Streaming Integration Tests
add_executable(NetworkStreamingTest
    integration/test_network_streaming.cpp
    ../src/playback/MpvPlaybackEngine.cpp
    ../src/network/UrlValidator.cpp
    ../src/core/Logger.cpp
)

target_link_libraries(NetworkStreamingTest PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Test
    ${MPV_LIBRARIES}
)

target_compile_definitions(NetworkStreamingTest PRIVATE HAVE_MPV=1)

if(WIN32)
    target_compile_definitions(NetworkStreamingTest PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(NetworkStreamingTest PRIVATE dwmapi shell32 user32)
endif()

add_test(NAME NetworkStreamingTest COMMAND NetworkStreamingTest)

# 13. Hardware-Decoding Profiles Integration Tests
add_executable(HwdecProfileTest
    integration/test_hwdec_profiles.cpp
    ../src/playback/MpvPlaybackEngine.cpp
    ../src/settings/SettingsManager.cpp
    ../src/core/Logger.cpp
)

target_link_libraries(HwdecProfileTest PRIVATE
    Qt6::Core
    Qt6::Test
    ${MPV_LIBRARIES}
)

target_compile_definitions(HwdecProfileTest PRIVATE HAVE_MPV=1)

if(WIN32)
    target_compile_definitions(HwdecProfileTest PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(HwdecProfileTest PRIVATE dwmapi shell32 user32)
endif()

add_test(NAME HwdecProfileTest COMMAND HwdecProfileTest)

# 14. A-B Repeat Unit Tests
add_executable(ABRepeatTest
    unit/test_ab_repeat.cpp
    ../src/playback/MpvPlaybackEngine.cpp
    ../src/ui/BottomControlBar.cpp
    ../src/core/Logger.cpp
)

target_link_libraries(ABRepeatTest PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Test
    ${MPV_LIBRARIES}
)

target_compile_definitions(ABRepeatTest PRIVATE HAVE_MPV=1)

if(WIN32)
    target_compile_definitions(ABRepeatTest PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(ABRepeatTest PRIVATE dwmapi shell32 user32)
endif()

add_test(NAME ABRepeatTest COMMAND ABRepeatTest)



