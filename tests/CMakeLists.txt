# Tests CMakeLists.txt

find_package(Qt6 REQUIRED COMPONENTS Test)

add_executable(UnitTests
    unit/test_settings.cpp
    ../src/settings/SettingsManager.cpp
    ../src/core/Logger.cpp
)

target_link_libraries(UnitTests PRIVATE
    Qt6::Core
    Qt6::Test
)

add_test(NAME SettingsTest COMMAND UnitTests)
