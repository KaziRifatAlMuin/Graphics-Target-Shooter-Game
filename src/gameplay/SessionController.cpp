#include "gameplay/SessionController.h"
#include <algorithm>
namespace shooter {
// Connect game, menu state, and saved rankings, requesting a name when the current mode needs one.
SessionController::SessionController(Game& g,Leaderboard& b):game(g),leaderboard(b) {
    ui.name=game.playerName;
    if (game.usesLevel()) ui.screen=Screen::Playing;
    refreshBoard();
    if(game.mode==GameMode::Challenge || (game.usesLevel() && ui.name.empty())) requestName(game.mode);
}
// Trim surrounding spaces and continue editing if the name is blank.
void SessionController::acceptName() {
    const auto first=ui.name.find_first_not_of(' '),last=ui.name.find_last_not_of(' ');
    ui.name=first==std::string::npos?"":ui.name.substr(first,last-first+1);
    ui.editingName=ui.name.empty();
}
// Accept printable ASCII characters up to 24 characters and refresh duplicate-name feedback.
void SessionController::type(unsigned int codepoint) {
    if (ui.editingName && codepoint>=32 && codepoint<=126 && ui.name.size()<24) {
        ui.name+=char(codepoint); ui.duplicateConfirmation=false;
        if (ui.screen==Screen::ChallengeName) checkChallengeName();
    }
}
// Remove the last typed character and recalculate name warnings.
void SessionController::backspace() {
    if (ui.editingName && !ui.name.empty()) {
        ui.name.pop_back(); ui.duplicateConfirmation=false;
        if (ui.screen==Screen::ChallengeName) checkChallengeName();
    }
}
// Look up the entered name in the current table and display its existing best result.
void SessionController::checkChallengeName() {
    const auto first=ui.name.find_first_not_of(' '),last=ui.name.find_last_not_of(' ');
    const auto name=first==std::string::npos?std::string{}:ui.name.substr(first,last-first+1);
    ui.nameExistsWarning=false;
    ui.existingRank=0;
    ui.existingScore=0;
    ui.existingLevels=0;
    for (std::size_t i=0;i<ui.rows.size();++i) {
        if (ui.rows[i].name==name) {
            ui.nameExistsWarning=true;
            ui.existingRank=int(i)+1;
            ui.existingScore=ui.rows[i].stats.score;
            ui.existingLevels=ui.rows[i].stats.levelsCleared;
            break;
        }
    }
}
// Start a named session and reset UI/input state so the launch click cannot fire a shot.
void SessionController::begin(GameMode mode,int level) {
    acceptName();
    if(ui.name.empty()) { requestName(mode); return; }
    game.playerName=ui.name; game.startMode(mode,level);
    ui.screen=Screen::Playing; ui.screenTime=0; ui.hasResult=false; ui.personalBest=false;
    if (!ui.saveFailed) ui.message.clear();
    pointerUnlocked=false; snapshotDirty=inputReset=true;
}
// Open name entry for the selected mode and refresh its leaderboard.
void SessionController::requestName(GameMode mode) {
    ui.pendingMode=mode; ui.screen=Screen::ChallengeName; ui.screenTime=0; ui.editingName=true;
    ui.duplicateConfirmation=false; ui.boardMode=mode; ui.hasResult=false;
    refreshBoard(); checkChallengeName(); inputReset=true;
}
// Route Developer to level selection or start the chosen playable mode.
void SessionController::launchNamedMode() {
    if(ui.pendingMode==GameMode::Developer) {
        acceptName(); game.playerName=ui.name; ui.screen=Screen::Developer; inputReset=true;
    } else begin(ui.pendingMode);
}
// Reload rankings and rank, preserving pending-save warnings if disk access fails.
void SessionController::refreshBoard() {
    try {
        leaderboard.load(); ui.rows=leaderboard.sorted(ui.boardMode);
        if (pendingSaves.empty()) { ui.saveFailed=false; ui.message.clear(); }
        ui.rank=leaderboard.rank(ui.hasResult?game.playerName:ui.name,ui.boardMode);
        if (!leaderboard.warning().empty()) ui.message=leaderboard.warning();
    } catch (const std::exception& e) { ui.message=e.what(); ui.saveFailed=true; }
    scroll(0);
}
// Retry queued results in order; retain an unsaved result when persistence throws an error.
void SessionController::saveResults() {
    while (!pendingSaves.empty()) {
        try {
            ui.personalBest=leaderboard.submit(pendingSaves.front());
            pendingSaves.erase(pendingSaves.begin()); ui.saveFailed=false; ui.message.clear();
        } catch (const std::exception& e) { ui.saveFailed=true; ui.message=e.what(); break; }
    }
    refreshBoard();
}
// Clamp the first displayed row to [0, rowCount-visibleRows] so scrolling stays within the table.
void SessionController::scroll(int rows) {
    ui.firstRow=std::clamp(ui.firstRow+rows,0,std::max(0,int(ui.rows.size())-leaderboardVisibleRows));
}
// Translate button/key actions into screen changes, mode launches, settings, or save retries.
void SessionController::action(Action a) {
    if (a>=Action::Level1 && a<=Action::Level7) { begin(GameMode::Developer,1+int(a)-int(Action::Level1)); return; }
    switch (a) {
    case Action::Start:
        requestName(GameMode::Challenge);
        break;
    case Action::ConfirmChallenge:
        if(ui.screen!=Screen::ChallengeName) break;
        if(ui.duplicateConfirmation) { action(Action::UseExistingName); break; }
        if(ui.name.find_first_not_of(' ')==std::string::npos) {
            ui.message="ENTER A NAME BEFORE STARTING."; ui.editingName=true; break;
        }
        acceptName(); refreshBoard(); checkChallengeName();
        if(ui.nameExistsWarning) { ui.duplicateConfirmation=true; break; }
        launchNamedMode(); break;
    case Action::UseExistingName:
        if(ui.screen==Screen::ChallengeName && ui.duplicateConfirmation) launchNamedMode();
        break;
    case Action::RewriteName:
        ui.name.clear(); ui.duplicateConfirmation=false; ui.message.clear();
        ui.editingName=true;
        checkChallengeName();
        break;
    case Action::Free:requestName(GameMode::Free);break;
    case Action::Practice:requestName(GameMode::Practice);break;
    case Action::BirdsEye:requestName(GameMode::BirdsEye);break;
    case Action::Developer:requestName(GameMode::Developer);break;
    case Action::Replay:
        if(game.mode==GameMode::Challenge) action(Action::Start);
        else begin(game.mode,game.levels.config.number);
        break;
    case Action::NextLevel:
        if (game.nextLevel()) { ui.screen=Screen::Playing; ui.hasResult=false; pointerUnlocked=false; inputReset=true; }
        break;
    case Action::Resume:ui.screen=Screen::Playing; pointerUnlocked=false; inputReset=true;break;
    case Action::Controls:ui.controlsReturn=ui.screen; ui.screen=Screen::Controls; ui.editingName=false;break;
    case Action::Back:
        ui.screen=ui.screen==Screen::Controls?ui.controlsReturn:ui.screen==Screen::Leaderboard?ui.boardReturn:Screen::Menu;
        ui.editingName=false;
        inputReset=true;break;
    case Action::Menu:ui.screen=ui.screen==Screen::Playing?Screen::Paused:Screen::Menu; ui.editingName=false; inputReset=true;break;
    case Action::Exit:exitRequested=true;break;
    case Action::Pistol:game.weapon=WeaponType::Pistol; game.recoil=0;break;
    case Action::Shotgun:game.weapon=WeaponType::Shotgun; game.recoil=0;break;
    case Action::Rifle:game.weapon=WeaponType::Rifle; game.recoil=0;break;
    case Action::DayNight:game.night=!game.night;break;
    case Action::Sound:game.soundEnabled=!game.soundEnabled;break;
    case Action::Overview:game.birdEye.overview(); pointerUnlocked=false; inputReset=true;break;
    case Action::EditName:ui.editingName=true; ui.duplicateConfirmation=false;break;
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
// Advance UI/game time, save newly completed results, and select the appropriate results screen.
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
