# Release verification — 2026-10-03

This report covers the compact HUD, texture/material system and packaged release described in [README](../README.md). Gameplay/level/world/camera/persistence implementation files were left unchanged; changes are in rendering, visual instance fields, UI, capture/benchmark support and build/documentation tooling.

## Automated checks

- Release build: MinGW C++17, `-O2`, successful with `build.bat`.
- `core_tests`: transforms, camera/projection, boundary/support geometry, target rings, pellet grouping, front-only contacts, movement/collision, weapons, respawn, audio and compact HUD button hit tests — passed.
- `challenge_tests`: all seven levels, reachability, movement restrictions, real target/NPC projectile contacts, exact scoring, transition gates, time, restart rollback and NPC bounds — passed.
- `mode_tests`: shared arena geometry, full Free countdown/respawn, Developer selection, Bird's-Eye picking/movement, completed-only submissions, pause, scrolling and failed-save retry — passed.
- `release_tests`: proportions/obstruction, fatal NPC contacts, grounded deaths, no respawn, effects/audio, name entry/confirmation, lighting, survivor redistribution and background snapshot behavior — passed.
- Separate persistence writer/reader processes preserved both mode records, quoted names, exact statistics and best-result comparison — passed.
- Packaged OpenGL `--modes-smoke-test`: menus, controls, all weapons/cameras, all seven Challenge/Developer levels, all five modes, target destruction, NPC penalties/deaths, 180-second Free session and 30-second target respawn, leaderboard reload and overhead observation — passed.
- Render checks compiled all shaders, checked visible geometry/OpenGL errors and distinct day/night/Flat/Gouraud/Phong outputs. The complete Challenge script cleared 46 targets with three bird and one human penalties for score 6400.
- Independent `verify_calc.ps1` recomputed transforms for 56,712 unique observed cube rows and verified all seven levels, modes, birds/humans and observation markers — passed. The renderer-only uniform cache does not change transforms.
- A deliberately truncated `metal.ppm` in an isolated package produced the expected named startup error instead of rendering invalid texture data.
- Root `leaderboard.csv` and `calc-init.csv` match their original Git versions. All checks used isolated calculation/leaderboard paths.

The accelerated GPU script injects hit events for progression. CPU suites separately test actual firing/contact behavior. Developer FPS visible in scripted screenshots is based on the fixed simulation timestep, not a performance measurement.

## Renderer measurements

Hardware reported by OpenGL: **AMD Radeon(TM) Graphics**, OpenGL **3.3.0 Core Profile Context 26.5.2.260413**.

The benchmark uses the real level-7 scene with **5,349 cube instances**, elevated camera, 1280 × 800, default Phong, VSync disabled, 40 warm-up frames followed by 200 measured frames per lighting condition. `glFinish` includes GPU completion. It measures the render pass only, excluding scene construction, simulation, UI, snapshots and asset loading.

Baseline was rebuilt using the repository's pre-change `Renderer.cpp`, `Renderer.h` and shader files with the same scene and benchmark harness. The final build adds textures plus cached rigs and redundant-uniform suppression. Runs were sequential with no concurrent test/build jobs during the reported comparison.

| Condition | Original median | Final median | Original p95 | Final p95 |
| --- | --- | --- | --- | --- |
| Day | 6.4048 ms | 5.3246 ms | 88.4720 ms | 81.1958 ms |
| Night | 7.3381 ms | 5.5977 ms | 87.0988 ms | 86.8123 ms |

Median render time improved approximately **16.9% by day and 23.7% at night**, despite texture sampling. This comparison does not show a rendering regression. However, both versions exhibit substantial long-tail stalls in this environment. Their cause was not isolated. These measurements cannot certify universally stutter-free gameplay, end-to-end input latency or performance on other hardware. No manual input-latency or listening test was claimed.

The texture array uses nine 256 × 256 RGB8 layers, approximately 2.25 MiB nominal storage including mipmaps, one scene-pass bind and one filtered sample per fragment. Actual driver allocation may be padded. No new lights, shadow passes, normal maps or high-resolution assets were introduced.

## Images and package

The final package was run with `--modes-smoke-test --capture .../release.ppm`. The complete capture set was refreshed after the uniform-cache optimization. [convert_captures.ps1](../tools/convert_captures.ps1) converts framebuffer PPMs losslessly to PNG without retouching. `docs/images/` contains 71 final captures covering menus/controls, all levels and modes, every weapon, target front/back and motion-level variations, NPCs, important props, materials, results and lighting.

The four `release-lighting-*` images isolate existing source groups using the real renderer and rig; normal gameplay uses the full rig. Three `release-shading-*` images compare the existing interpolation modes. Close views reposition the documentation camera only. README links use relative paths.

[release-artifacts.json](release-artifacts.json) records executable and screenshot SHA-256 hashes. `build/` contains only `release/` with **17 runtime files**: executable, seven shader files and nine textures. Older build trees, snapshots, screenshots and generated files were removed. No compiler intermediates or test records remain in the package.

For reproduction commands and implementation references, see [README](../README.md).