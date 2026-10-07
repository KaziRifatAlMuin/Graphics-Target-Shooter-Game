# Generated object demonstrations

Run `mingw32-make`, `build.bat`, or build CMake target `main`. The demonstration generator runs on every build, including when compilation is up to date. Manual regeneration: `demonstrateObject.exe <project-root>`. Requires an OpenGL 3.3 context.

One directory per unique assembly/component type; repeated instances are not duplicated. All seven levels and Practice are inspected. Alternate poses and temporary states share their type's directory. Values come from actual builders; fixed seeds, call order and simulation times make examples repeatable on the same toolchain/GPU. GPU rasterization may differ slightly across drivers. All PNGs are real lossless renderer captures, 480 x 360. No boolean cube union exists: assemblies are overlapping independent cube draw calls. UI text/menus and the fullscreen procedural sky are not SceneObjects; sky is documented in lighting.

| Object type | Parts in representative | Guide |
| --- | ---: | --- |
| arena-floor-slab | 1 | [Open](arena-floor-slab/arena-floor-slab.md) |
| arena-painted-route-marker | 1 | [Open](arena-painted-route-marker/arena-painted-route-marker.md) |
| bird | 10 | [Open](bird/bird.md) |
| blood-blood-droplet | 1 | [Open](blood-blood-droplet/blood-blood-droplet.md) |
| boundary-corner-cap | 1 | [Open](boundary-corner-cap/boundary-corner-cap.md) |
| boundary-east-wall | 1 | [Open](boundary-east-wall/boundary-east-wall.md) |
| boundary-masonry-string-course | 1 | [Open](boundary-masonry-string-course/boundary-masonry-string-course.md) |
| boundary-north-top-block | 1 | [Open](boundary-north-top-block/boundary-north-top-block.md) |
| boundary-north-wall | 1 | [Open](boundary-north-wall/boundary-north-wall.md) |
| boundary-sheared-stone-support | 1 | [Open](boundary-sheared-stone-support/boundary-sheared-stone-support.md) |
| boundary-side-top-block | 1 | [Open](boundary-side-top-block/boundary-side-top-block.md) |
| boundary-south-top-block | 1 | [Open](boundary-south-top-block/boundary-south-top-block.md) |
| boundary-south-wall | 1 | [Open](boundary-south-wall/boundary-south-wall.md) |
| boundary-thick-corner-tower | 1 | [Open](boundary-thick-corner-tower/boundary-thick-corner-tower.md) |
| boundary-west-wall | 1 | [Open](boundary-west-wall/boundary-west-wall.md) |
| camera-reference | 2 | [Open](camera-reference/camera-reference.md) |
| cargo-crate | 4 | [Open](cargo-crate/cargo-crate.md) |
| celebration-victory-confetti | 1 | [Open](celebration-victory-confetti/celebration-victory-confetti.md) |
| environment-sun-visual | 1 | [Open](environment-sun-visual/environment-sun-visual.md) |
| equipment-table | 5 | [Open](equipment-table/equipment-table.md) |
| hit-effect-break-fragment | 1 | [Open](hit-effect-break-fragment/hit-effect-break-fragment.md) |
| human | 21 | [Open](human/human.md) |
| lamp-post | 5 | [Open](lamp-post/lamp-post.md) |
| player | 4 | [Open](player/player.md) |
| projectile-assault-rifle | 1 | [Open](projectile-assault-rifle/projectile-assault-rifle.md) |
| projectile-pistol | 1 | [Open](projectile-pistol/projectile-pistol.md) |
| projectile-shotgun | 1 | [Open](projectile-shotgun/projectile-shotgun.md) |
| stadium-floodlight | 12 | [Open](stadium-floodlight/stadium-floodlight.md) |
| target | 38 | [Open](target/target.md) |
| wall-lamp | 5 | [Open](wall-lamp/wall-lamp.md) |
| weapon-pistol | 11 | [Open](weapon-pistol/weapon-pistol.md) |
| weapon-rifle | 15 | [Open](weapon-rifle/weapon-rifle.md) |
| weapon-shotgun | 14 | [Open](weapon-shotgun/weapon-shotgun.md) |

## Component coverage

| Observed game type / component | Demonstration directory |
| --- | --- |
| Arena / Floor slab | [arena-floor-slab](arena-floor-slab/arena-floor-slab.md) |
| Arena / Painted route marker | [arena-painted-route-marker](arena-painted-route-marker/arena-painted-route-marker.md) |
| Bird / Beak | [bird](bird/bird.md) |
| Bird / Body | [bird](bird/bird.md) |
| Bird / Eye | [bird](bird/bird.md) |
| Bird / Head | [bird](bird/bird.md) |
| Bird / OtherEye | [bird](bird/bird.md) |
| Bird / Tail | [bird](bird/bird.md) |
| Bird / WingLeft | [bird](bird/bird.md) |
| Bird / WingRight | [bird](bird/bird.md) |
| Bird / WingTipLeft | [bird](bird/bird.md) |
| Bird / WingTipRight | [bird](bird/bird.md) |
| Blood / Blood droplet | [blood-blood-droplet](blood-blood-droplet/blood-blood-droplet.md) |
| Boundary / Corner cap | [boundary-corner-cap](boundary-corner-cap/boundary-corner-cap.md) |
| Boundary / East wall | [boundary-east-wall](boundary-east-wall/boundary-east-wall.md) |
| Boundary / Masonry string course | [boundary-masonry-string-course](boundary-masonry-string-course/boundary-masonry-string-course.md) |
| Boundary / North top block | [boundary-north-top-block](boundary-north-top-block/boundary-north-top-block.md) |
| Boundary / North wall | [boundary-north-wall](boundary-north-wall/boundary-north-wall.md) |
| Boundary / Sheared stone support | [boundary-sheared-stone-support](boundary-sheared-stone-support/boundary-sheared-stone-support.md) |
| Boundary / Side top block | [boundary-side-top-block](boundary-side-top-block/boundary-side-top-block.md) |
| Boundary / South top block | [boundary-south-top-block](boundary-south-top-block/boundary-south-top-block.md) |
| Boundary / South wall | [boundary-south-wall](boundary-south-wall/boundary-south-wall.md) |
| Boundary / Thick corner tower | [boundary-thick-corner-tower](boundary-thick-corner-tower/boundary-thick-corner-tower.md) |
| Boundary / West wall | [boundary-west-wall](boundary-west-wall/boundary-west-wall.md) |
| Camera reference / Observation ground marker | [camera-reference](camera-reference/camera-reference.md) |
| Camera reference / Observation heading | [camera-reference](camera-reference/camera-reference.md) |
| Cargo / Crate | [cargo-crate](cargo-crate/cargo-crate.md) |
| Cargo / Crate band | [cargo-crate](cargo-crate/cargo-crate.md) |
| Cargo / Crate inventory plate | [cargo-crate](cargo-crate/cargo-crate.md) |
| Celebration / Victory confetti | [celebration-victory-confetti](celebration-victory-confetti/celebration-victory-confetti.md) |
| Environment / Display identification plate | [equipment-table](equipment-table/equipment-table.md) |
| Environment / Equipment mat | [equipment-table](equipment-table/equipment-table.md) |
| Environment / Equipment table | [equipment-table](equipment-table/equipment-table.md) |
| Environment / Sun visual | [environment-sun-visual](environment-sun-visual/environment-sun-visual.md) |
| Hit effect / Break fragment | [hit-effect-break-fragment](hit-effect-break-fragment/hit-effect-break-fragment.md) |
| Human / Belt | [human](human/human.md) |
| Human / Eye-1 | [human](human/human.md) |
| Human / Eye1 | [human](human/human.md) |
| Human / Forearm-1 | [human](human/human.md) |
| Human / Forearm1 | [human](human/human.md) |
| Human / Hair | [human](human/human.md) |
| Human / Hand-1 | [human](human/human.md) |
| Human / Hand1 | [human](human/human.md) |
| Human / Head | [human](human/human.md) |
| Human / Neck | [human](human/human.md) |
| Human / Nose | [human](human/human.md) |
| Human / Shin-1 | [human](human/human.md) |
| Human / Shin1 | [human](human/human.md) |
| Human / Shoe-1 | [human](human/human.md) |
| Human / Shoe1 | [human](human/human.md) |
| Human / Thigh-1 | [human](human/human.md) |
| Human / Thigh1 | [human](human/human.md) |
| Human / Torso | [human](human/human.md) |
| Human / UpperArm-1 | [human](human/human.md) |
| Human / UpperArm1 | [human](human/human.md) |
| Human / VestStripe | [human](human/human.md) |
| Lighting / Concrete footing | [stadium-floodlight](stadium-floodlight/stadium-floodlight.md) |
| Lighting / Floodlight crossbar | [stadium-floodlight](stadium-floodlight/stadium-floodlight.md) |
| Lighting / Floodlight housing | [stadium-floodlight](stadium-floodlight/stadium-floodlight.md) |
| Lighting / Footing | [lamp-post](lamp-post/lamp-post.md) |
| Lighting / Head support | [stadium-floodlight](stadium-floodlight/stadium-floodlight.md) |
| Lighting / Lamp cap | [wall-lamp](wall-lamp/wall-lamp.md) |
| Lighting / Lamp housing | [wall-lamp](wall-lamp/wall-lamp.md) |
| Lighting / Lamp post | [lamp-post](lamp-post/lamp-post.md) |
| Lighting / Lens | [stadium-floodlight](stadium-floodlight/stadium-floodlight.md) |
| Lighting / Stadium tower | [stadium-floodlight](stadium-floodlight/stadium-floodlight.md) |
| Lighting / Wall bracket | [wall-lamp](wall-lamp/wall-lamp.md) |
| Lighting / Wall mounting plate | [wall-lamp](wall-lamp/wall-lamp.md) |
| Player / Shooter body | [player](player/player.md) |
| Player / Shooter head | [player](player/player.md) |
| Player / Shooter leg | [player](player/player.md) |
| Projectile / ASSAULT RIFLE | [projectile-assault-rifle](projectile-assault-rifle/projectile-assault-rifle.md) |
| Projectile / PISTOL | [projectile-pistol](projectile-pistol/projectile-pistol.md) |
| Projectile / SHOTGUN | [projectile-shotgun](projectile-shotgun/projectile-shotgun.md) |
| Target / Base warning stripe | [target](target/target.md) |
| Target / Cube-built six-ring plate | [target](target/target.md) |
| Target / Respawn warning | [target](target/target.md) |
| Target / Stand | [target](target/target.md) |
| Target / Stand base | [target](target/target.md) |
| Target / Stand brace | [target](target/target.md) |
| Weapon / BARREL | [weapon-shotgun](weapon-shotgun/weapon-shotgun.md) |
| Weapon / BODY | [weapon-shotgun](weapon-shotgun/weapon-shotgun.md) |
| Weapon / DETAIL_0 | [weapon-shotgun](weapon-shotgun/weapon-shotgun.md) |
| Weapon / DETAIL_1 | [weapon-shotgun](weapon-shotgun/weapon-shotgun.md) |
| Weapon / DETAIL_2 | [weapon-shotgun](weapon-shotgun/weapon-shotgun.md) |
| Weapon / DETAIL_3 | [weapon-shotgun](weapon-shotgun/weapon-shotgun.md) |
| Weapon / FORE_END | [weapon-shotgun](weapon-shotgun/weapon-shotgun.md) |
| Weapon / FRONT_SIGHT | [weapon-shotgun](weapon-shotgun/weapon-shotgun.md) |
| Weapon / GRIP | [weapon-shotgun](weapon-shotgun/weapon-shotgun.md) |
| Weapon / GUARD_FRONT | [weapon-shotgun](weapon-shotgun/weapon-shotgun.md) |
| Weapon / MAGAZINE | [weapon-rifle](weapon-rifle/weapon-rifle.md) |
| Weapon / MUZZLE_FLASH | [weapon-shotgun](weapon-shotgun/weapon-shotgun.md) |
| Weapon / MUZZLE_INSET | [weapon-shotgun](weapon-shotgun/weapon-shotgun.md) |
| Weapon / REAR_SIGHT | [weapon-shotgun](weapon-shotgun/weapon-shotgun.md) |
| Weapon / SIGHT | [weapon-pistol](weapon-pistol/weapon-pistol.md) |
| Weapon / STOCK | [weapon-shotgun](weapon-shotgun/weapon-shotgun.md) |
| Weapon / TRIGGER_GUARD | [weapon-shotgun](weapon-shotgun/weapon-shotgun.md) |
