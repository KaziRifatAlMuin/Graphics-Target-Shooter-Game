#include "gameplay/SessionController.h"
#include <algorithm>
namespace shooter {
SessionController::SessionController(Game& g,Leaderboard& b):game(g),leaderboard(b) {
    ui.name=game.playerName;
    if (game.usesLevel()) ui.screen=Screen::Playing;
    refreshBoard();
}
void SessionController::acceptName() {
    const auto first=ui.name.find_first_not_of(' '),last=ui.name.find_last_not_of(' ');
    ui.name=first==std::string::npos?"Player":ui.name.substr(first,last-first+1);
    ui.editingName=false;
}
void SessionController::type(unsigned int codepoint) {
    if (ui.editingName && codepoint>=32 && codepoint<=126 && ui.name.size()<24) ui.name+=char(codepoint);
}
void SessionController::backspace() { if (ui.editingName && !ui.name.empty()) ui.name.pop_back(); }
void SessionController::begin(GameMode mode,int level) {
    acceptName(); game.playerName=ui.name; game.startMode(mode,level);
    ui.screen=Screen::Playing; ui.screenTime=0; ui.hasResult=false; ui.personalBest=false;
    if (!ui.saveFailed) ui.message.clear();
    pointerUnlocked=false; snapshotDirty=inputReset=true;
}
void SessionController::refreshBoard() {
    try {
        leaderboard.load(); ui.rows=leaderboard.sorted(ui.boardMode);
        if (pendingSaves.empty()) { ui.saveFailed=false; ui.message.clear(); }
        ui.rank=leaderboard.rank(ui.hasResult?game.playerName:ui.name,ui.boardMode);
        if (!leaderboard.warning().empty()) ui.message=leaderboard.warning();
    } catch (const std::exception& e) { ui.message=e.what(); ui.saveFailed=true; }
    scroll(0);
}
void SessionController::saveResults() {
    while (!pendingSaves.empty()) {
        try {
            ui.personalBest=leaderboard.submit(pendingSaves.front());
            pendingSaves.erase(pendingSaves.begin()); ui.saveFailed=false; ui.message.clear();
        } catch (const std::exception& e) { ui.saveFailed=true; ui.message=e.what(); break; }
    }
    refreshBoard();
}
void SessionController::scroll(int rows) {
    ui.firstRow=std::clamp(ui.firstRow+rows,0,std::max(0,int(ui.rows.size())-leaderboardVisibleRows));
}
void SessionController::action(Action a) {
    if (a>=Action::Level1 && a<=Action::Level7) { begin(GameMode::Developer,1+int(a)-int(Action::Level1)); return; }
    switch (a) {
    case Action::Start:begin(GameMode::Challenge);break;
    case Action::Free:begin(GameMode::Free);break;
    case Action::Practice:begin(GameMode::Practice);break;
    case Action::BirdsEye:begin(GameMode::BirdsEye);break;
    case Action::Developer:ui.screen=Screen::Developer; ui.editingName=false; inputReset=true;break;
    case Action::Replay:begin(game.mode,game.mode==GameMode::Challenge?1:game.levels.config.number);break;
    case Action::NextLevel:
        if (game.nextLevel()) { ui.screen=Screen::Playing; ui.hasResult=false; pointerUnlocked=false; inputReset=true; }
        break;
    case Action::Resume:ui.screen=Screen::Playing; pointerUnlocked=false; inputReset=true;break;
    case Action::Controls:ui.controlsReturn=ui.screen; ui.screen=Screen::Controls; ui.editingName=false;break;
    case Action::Back:
        ui.screen=ui.screen==Screen::Controls?ui.controlsReturn:ui.screen==Screen::Leaderboard?ui.boardReturn:Screen::Menu;
        inputReset=true;break;
    case Action::Menu:ui.screen=ui.screen==Screen::Playing?Screen::Paused:Screen::Menu; ui.editingName=false; inputReset=true;break;
    case Action::Exit:exitRequested=true;break;
    case Action::Pistol:game.weapon=WeaponType::Pistol; game.recoil=0;break;
    case Action::Shotgun:game.weapon=WeaponType::Shotgun; game.recoil=0;break;
    case Action::Rifle:game.weapon=WeaponType::Rifle; game.recoil=0;break;
    case Action::DayNight:game.night=!game.night;break;
    case Action::Sound:game.soundEnabled=!game.soundEnabled;break;
    case Action::Overview:game.birdEye.overview(); pointerUnlocked=false; inputReset=true;break;
    case Action::EditName:ui.editingName=!ui.editingName;break;
    case Action::Leaderboard:
        ui.boardReturn=ui.screen; ui.screen=Screen::Leaderboard; ui.editingName=false; ui.hasResult=false; ui.firstRow=0; refreshBoard();break;
    case Action::BoardFree:ui.boardMode=GameMode::Free; ui.firstRow=0;refreshBoard();break;
    case Action::BoardChallenge:ui.boardMode=GameMode::Challenge; ui.firstRow=0;refreshBoard();break;
    case Action::ScrollUp:scroll(-leaderboardVisibleRows);break;
    case Action::ScrollDown:scroll(leaderboardVisibleRows);break;
    case Action::RetrySave:saveResults();break;
    default:break;
    }
    if (a!=Action::None) snapshotDirty=true;
}
void SessionController::update(float dt) {
    ui.animationTime+=dt;
    if (ui.screen!=animatedScreen) { animatedScreen=ui.screen; ui.screenTime=0; }
    ui.screenTime+=dt;
    if (ui.screen!=Screen::Playing && !resultScreen(ui.screen)) return;
    game.update(dt);
    if (auto result=game.takeResult()) {
        pendingSaves.push_back(*result); ui.hasResult=true; ui.boardMode=result->mode; ui.firstRow=0;
        saveResults();
    }
    if (!game.usesLevel() || game.mode==GameMode::BirdsEye) return;
    Screen next=ui.screen;
    if (game.mode==GameMode::Free && game.levels.stage==LevelStage::Finished) next=Screen::FreeComplete;
    else if (game.mode==GameMode::Developer && (game.levels.stage==LevelStage::Complete || game.levels.stage==LevelStage::Finished)) next=Screen::LevelComplete;
    else if (game.mode==GameMode::Challenge) {
        if (game.levels.stage==LevelStage::Complete) next=Screen::LevelComplete;
        if (game.levels.stage==LevelStage::Finished) next=Screen::Victory;
    }
    if (next!=ui.screen) { ui.screen=next; snapshotDirty=inputReset=true; }
}
}
