cmake_minimum_required(VERSION 3.15)
project(TargetShooter VERSION 1.0.0 LANGUAGES C CXX)

# Require C++17 for filesystem, optional values, and the other language features used by the game.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(Threads REQUIRED)

# Compile shared simulation, scene, and UI code once for the game, tools, and tests.
add_library(scene_core
    src/world/Arena.cpp
    src/gameplay/GameMode.cpp
    src/persistence/CsvFile.cpp
    src/persistence/Leaderboard.cpp
    src/camera/BirdEyeCamera.cpp
    src/gameplay/SessionController.cpp
    src/ui/ModeUI.cpp
    src/ui/LeaderboardUI.cpp
    src/camera/Camera.cpp
    src/gameplay/Game.cpp
    src/gameplay/GameScene.cpp
    src/gameplay/Effects.cpp
    src/ui/Presentation.cpp
    src/gameplay/Weapon.cpp
    src/ui/Interface.cpp
    src/world/Lighting.cpp
    src/audio/Sound.cpp
    src/core/Collision.cpp
    src/persistence/CsvLogger.cpp
    src/persistence/SnapshotWriter.cpp
    src/world/Cargo.cpp
    src/world/Environment.cpp
    src/gameplay/Target.cpp
    src/gameplay/Projectile.cpp
    src/gameplay/Challenge.cpp
    src/gameplay/Movement.cpp
    src/gameplay/LevelManager.cpp
    src/world/LevelWorld.cpp
    src/gameplay/ScoreSystem.cpp
    src/gameplay/Npc.cpp
    src/gameplay/Bird.cpp
    src/gameplay/Human.cpp
    src/levels/LevelBase.cpp
    src/levels/Level1.cpp
    src/levels/Level2.cpp
    src/levels/Level3.cpp
    src/levels/Level4.cpp
    src/levels/Level5.cpp
    src/levels/Level6.cpp
    src/levels/Level7.cpp
)
target_include_directories(scene_core PUBLIC src)
target_link_libraries(scene_core PUBLIC Threads::Threads)

# Optional documentation audit; uses the real scene builders without opening a window.
add_executable(scene_reference EXCLUDE_FROM_ALL tools/scene_reference.cpp)
target_link_libraries(scene_reference PRIVATE scene_core)

# Combine the application and GPU renderer with the generated OpenGL function loader.
add_executable(main src/main.cpp src/core/Application.cpp src/testing/ModeSmoke.cpp src/rendering/Renderer.cpp src/rendering/TextureCache.cpp src/third_party/glad.c)
target_include_directories(main PRIVATE include)
target_compile_definitions(main PRIVATE SHOOTER_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(main PRIVATE scene_core)

# Documentation uses the exact same builders, renderer, shaders and textures.
add_executable(demonstrateObject objects/demonstrateObject.cpp lighting/demonstrateLighting.cpp
    tools/Demonstration.cpp src/rendering/Renderer.cpp src/rendering/TextureCache.cpp src/third_party/glad.c)
target_include_directories(demonstrateObject PRIVATE include)
target_link_libraries(demonstrateObject PRIVATE scene_core)
add_custom_target(generate_demonstrations
    COMMAND $<TARGET_FILE:demonstrateObject> "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS demonstrateObject
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Regenerating object and lighting demonstrations (requires OpenGL 3.3)"
    VERBATIM)
add_dependencies(main generate_demonstrations)

# Choose platform libraries; the bundled Windows GLFW archive requires the MinGW toolchain.
if(WIN32)
    target_link_libraries(scene_core PUBLIC winmm)
    if(NOT MINGW)
        message(FATAL_ERROR "The bundled GLFW library requires 64-bit MinGW. Configure with -G \"MinGW Makefiles\".")
    endif()
    target_link_libraries(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw3.a" opengl32 gdi32)
    target_link_libraries(demonstrateObject PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw3.a" opengl32 gdi32)
else()
    find_package(OpenGL REQUIRED)
    find_package(glfw3 3.3 REQUIRED)
    target_link_libraries(main PRIVATE glfw OpenGL::GL)
    target_link_libraries(demonstrateObject PRIVATE glfw OpenGL::GL)
endif()

if(MINGW OR CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(scene_core PRIVATE -Wall -Wextra)
    target_compile_options(main PRIVATE -Wall -Wextra)
    target_compile_options(demonstrateObject PRIVATE -Wall -Wextra)
endif()

# Runs even when compilation is already up to date. No window or GPU is needed.
add_custom_target(generate_calculations
    COMMAND $<TARGET_FILE:main> --export-calc
    DEPENDS main
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Regenerating calc.csv from the current scene"
    VERBATIM)

# Register automated math, gameplay, persistence, and calculation-export checks with CTest.
include(CTest)
if(BUILD_TESTING)
    add_executable(release_tests tests/release_tests.cpp)
    target_link_libraries(release_tests PRIVATE scene_core)
    add_test(NAME phase4_release COMMAND release_tests "${CMAKE_CURRENT_BINARY_DIR}/release-test.csv")
    add_executable(core_tests tests/core_tests.cpp)
    target_link_libraries(core_tests PRIVATE scene_core)
    add_test(NAME core_math_and_scene COMMAND core_tests)
    add_executable(challenge_tests tests/challenge_tests.cpp)
    target_link_libraries(challenge_tests PRIVATE scene_core)
    add_test(NAME challenge_rules COMMAND challenge_tests "${CMAKE_CURRENT_BINARY_DIR}/challenge-calc.csv")
    add_executable(mode_tests tests/mode_tests.cpp)
    target_link_libraries(mode_tests PRIVATE scene_core)
    add_test(NAME phase3_modes_and_persistence COMMAND mode_tests "${CMAKE_CURRENT_BINARY_DIR}/mode-test.csv")
    add_test(NAME persistence_write_process COMMAND mode_tests "${CMAKE_CURRENT_BINARY_DIR}/restart-test.csv" --persist-write)
    add_test(NAME persistence_read_process COMMAND mode_tests "${CMAKE_CURRENT_BINARY_DIR}/restart-test.csv" --persist-read)
    set_tests_properties(persistence_read_process PROPERTIES DEPENDS persistence_write_process)
    add_test(NAME calculation_export COMMAND main --export-calc --calc "${CMAKE_CURRENT_BINARY_DIR}/test-calc.csv")
    if(WIN32)
        add_test(NAME calculation_math COMMAND powershell -NoProfile -ExecutionPolicy Bypass
            -File "${CMAKE_CURRENT_SOURCE_DIR}/tests/verify_calc.ps1"
            -Path "${CMAKE_CURRENT_BINARY_DIR}/test-calc.csv")
        set_tests_properties(calculation_math PROPERTIES DEPENDS calculation_export)
        add_test(NAME challenge_calculation_math COMMAND powershell -NoProfile -ExecutionPolicy Bypass
            -File "${CMAKE_CURRENT_SOURCE_DIR}/tests/verify_calc.ps1"
            -Path "${CMAKE_CURRENT_BINARY_DIR}/challenge-calc.csv" -RequireChallengeCoverage)
        set_tests_properties(challenge_calculation_math PROPERTIES DEPENDS challenge_rules)
        add_test(NAME mode_calculation_math COMMAND powershell -NoProfile -ExecutionPolicy Bypass
            -File "${CMAKE_CURRENT_SOURCE_DIR}/tests/verify_calc.ps1"
            -Path "${CMAKE_CURRENT_BINARY_DIR}/mode-test.csv.calc.csv" -RequireModeCoverage)
        set_tests_properties(mode_calculation_math PROPERTIES DEPENDS phase3_modes_and_persistence)
    endif()
    # GPU smoke test is explicit: main --smoke-test (needs OpenGL 3.3).
endif()
