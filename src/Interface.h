#pragma once
#include "Game.h"

namespace shooter {
enum class Screen { Menu, Controls, Playing, Paused };
enum class Action { None, Start, Resume, Controls, Back, Menu, Exit, Pistol, Shotgun, Rifle };
struct UiVertex { float x,y,r,g,b; };
struct Button { float x,y,w,h; const char* text; Action action; };
std::vector<Button> screenButtons(Screen screen);
Action clickedAction(Screen screen, float x, float y);
std::vector<UiVertex> buildInterface(const Game& game, Screen screen, float mouseX, float mouseY, bool pointerFree);
}
