# Report image sources

`report/` contains the 176 selected images used by `../report.tex` and `../report-sections.tex`, copied by `../prepare.ps1`.

- `report/objects/`: original OpenGL construction captures from the repository's `objects/` guides. Cube parameters and numbered assembly frames come from the game builders.
- `report/lighting/`: original captures from `lighting/`, rendered with the game's shaders, rig, textures and explicit diagnostic masks.
- `report/game/`: original release gameplay captures. Levels 1--7 use `docs/images/release-level-1.png` through `release-level-7.png`; the final-level results image is also included. HUD values and visible hit feedback belong to the recorded Challenge session. Other release captures illustrate cameras, materials and equipment.
- `report/textures/`: lossless PNG conversions of the original `assets/textures/*.ppm` pixel arrays. Conversion changes the container format, not the image pixels.

The machine-readable provenance and SHA-256 hashes are in [report-image-manifest.csv](../report-image-manifest.csv). Report source names deliberately retain their original directory structure to prevent collisions between repeated names such as `complete.png`.

`kuet_logo.png` is the institutional logo retrieved from KUET's official [admissions website](https://admission.kuet.ac.bd/index.html), specifically [its logo asset](https://admission.kuet.ac.bd/static/logo.png), on 8 October 2026. It is used on the supplied university report cover, not presented as an original project graphic. The original template is retained in `../template.tex`.

All other images are project outputs. The report bibliography cites the project, the Khronos OpenGL 3.3 specification, GLFW's documentation, the original Phong and Gouraud papers. It does not reuse the unrelated sentiment-classification content or references from the sample template.

The cover image, `cover-level-7-night.png`, is a 1280 x 720 capture of Level 7 through the actual player camera at (0,1.7,-12), yaw -82 degrees, pitch 2 degrees, 60-degree perspective, night lighting and Phong shading. The rifle and all world objects are rendered by the game builders; no UI overlay is included. Capture source: `tools/capture_report_levels.cpp`.
