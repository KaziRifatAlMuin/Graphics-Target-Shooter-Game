# Report image sources

`report/` contains the 176 selected images used by `../report.tex` and `../report-sections.tex`, copied by `../../tools/prepare_report.ps1`.

- `report/objects/`: original OpenGL construction captures from the repository's `objects/` guides. Cube parameters and numbered assembly frames come from the game builders.
- `report/lighting/`: original captures from `lighting/`, rendered with the game's shaders, rig, textures and explicit diagnostic masks.
- `report/game/`: original release gameplay captures. Levels 1--7 use `docs/images/release-level-1.png` through `release-level-7.png`; the final-level results image is also included. HUD values and visible hit feedback belong to the recorded Challenge session. Other release captures illustrate cameras, materials and equipment.
- `report/textures/`: lossless PNG conversions of the original `assets/textures/*.ppm` pixel arrays. Conversion changes the container format, not the image pixels.

The machine-readable provenance and SHA-256 hashes are in [report-image-manifest.csv](../report-image-manifest.csv). Report source names deliberately retain their original directory structure to prevent collisions between repeated names such as `complete.png`.

`kuet_logo.png` is the institutional logo retrieved from KUET's official [admissions website](https://admission.kuet.ac.bd/index.html), specifically [its logo asset](https://admission.kuet.ac.bd/static/logo.png), on 8 October 2026. It is used on the supplied university report cover, not presented as an original project graphic. The original template is retained in `../template.tex`.

All other images are project outputs. The report bibliography cites the project, the Khronos OpenGL 3.3 specification, GLFW's documentation, the original Phong and Gouraud papers, and KUET. It does not reuse the unrelated sentiment-classification content or references from the sample template.
