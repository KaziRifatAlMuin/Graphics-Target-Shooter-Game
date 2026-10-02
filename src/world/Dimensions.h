#pragma once
namespace shooter::dimensions {
// World meters. Keep people, cargo and camera references on one documented scale.
inline constexpr float humanHeight=2.1f;
inline constexpr float humanMinHeight=2.05f, humanMaxHeight=2.20f;
inline constexpr float crateSize=humanHeight/3;
inline constexpr float eyeHeight=1.7f;
}
