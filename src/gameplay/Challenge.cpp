#include "gameplay/Game.h"
#include "world/LevelWorld.h"
#include <algorithm>
namespace shooter {
bool Game::gameplayActive() const { return !usesLevel() || levels.stage==LevelStage::Active; }
int Game::remainingTargets() const {
    return int(std::count_if(targets.begin(),targets.end(),[](const Target& t) { return !t.eliminated && t.respawn<=0; }));
}
void Game::startChallenge(int firstLevel) {
    startMode(GameMode::Challenge,firstLevel);
}
void Game::startMode(GameMode selected,int level) {
    if (selected==GameMode::Practice) { reset(); return; }
    const int number=(selected==GameMode::Free || selected==GameMode::BirdsEye)?7:level;
    levels.load(number); mode=selected; static_cast<RunStats&>(*this)=RunStats{};
    challengeFromStart=selected==GameMode::Challenge && level==1;
    pendingResult.reset(); freeRemaining=freeSessionSeconds; birdEye.reset();
    levels.completedStats=RunStats{}; weapon=WeaponType::Pistol;
    loadCurrentLevel();
    if (mode==GameMode::BirdsEye) levels.stage=LevelStage::Active;
}
std::optional<EligibleResult> Game::takeResult() {
    auto result=pendingResult; pendingResult.reset(); return result;
}
void Game::loadCurrentLevel() {
    staticObjects=createLevelWorld(levels.config); targets=levels.config.targets;
    for (auto& target:targets) updateTarget(target,0,0);
    player.position={0,1.7f,-5}; player.yaw=-90; player.pitch=3.8f;
    freeCamera=Camera{}; cameraMode=1; projectiles.clear(); debris.clear(); soundEvents.clear();
    scoreFeedback.clear(); cooldown=recoil=feedbackTime=dangerTime=0; lastRing=-1;
    soundEvents.push_back(SoundEvent::Start);
    birds.clear(); humans.clear();
    const auto& c=levels.config;
    const int birdCount=c.birdsPerTarget?int(targets.size())*c.birdsPerTarget:c.birds;
    for (int i=0;i<birdCount;++i) {
        const std::size_t zone=std::size_t(i)%targets.size();
        birds.push_back(createBird(zone,targets[zone].base,c.seed+unsigned(i)*193+47));
    }
    for (std::size_t zone=0;zone<targets.size();++zone) for (int i=0;i<c.humansPerTarget;++i)
        humans.push_back(createHuman(zone,targets[zone].base,c.seed+unsigned(zone)*547+unsigned(i)*193,staticObjects,targets));
}
bool Game::nextLevel() {
    if (mode!=GameMode::Challenge || !levels.advance()) return false;
    loadCurrentLevel(); return true;
}
void Game::restartLevel() {
    if (mode==GameMode::Free || mode==GameMode::BirdsEye || mode==GameMode::Developer) { startMode(mode,levels.config.number); return; }
    if (!usesLevel() || (levels.stage!=LevelStage::Active && levels.stage!=LevelStage::Intro)) return;
    static_cast<RunStats&>(*this)=levels.completedStats;
    levels.load(levels.config.number); loadCurrentLevel();
}
void Game::updateNpcs(float dt) {
    std::vector<std::size_t> active;
    for (std::size_t i=0;i<targets.size();++i) if (!targets[i].eliminated) active.push_back(i);
    if (active.empty()) return;
    for (std::size_t i=0;i<birds.size();++i) {
        auto& b=birds[i];
        if (targets[b.zone].eliminated) {
            if (levels.config.birdsPerTarget) { b.active=false; continue; }
            const auto remembered=std::move(b.penalizedShots);
            b=createBird(active[i%active.size()],targets[active[i%active.size()]].base,b.random);
            b.penalizedShots=remembered;
        }
        updateBird(b,dt,staticObjects);
    }
    for (auto& h:humans) {
        if (targets[h.zone].eliminated) { h.active=false; continue; }
        updateHuman(h,dt,staticObjects,targets);
    }
}
std::vector<NpcCollider> Game::npcColliders() const {
    std::vector<NpcCollider> result;
    for (std::size_t i=0;i<birds.size();++i) if (birds[i].active) {
        NpcCollider c{false,i,{}};
        for (const auto& part:createBirdObjects(birds[i],i)) c.parts.push_back(part.transform);
        result.push_back(std::move(c));
    }
    for (std::size_t i=0;i<humans.size();++i) if (humans[i].active) {
        NpcCollider c{true,i,{}};
        for (const auto& part:createHumanObjects(humans[i],i)) c.parts.push_back(part.transform);
        result.push_back(std::move(c));
    }
    return result;
}
void Game::applyNpcHit(bool human,std::size_t index,std::uint64_t shotId) {
    if (!usesLevel() || mode==GameMode::BirdsEye || !gameplayActive()) return;
    NpcState& npc=human?static_cast<NpcState&>(humans.at(index)):static_cast<NpcState&>(birds.at(index));
    if (!npc.active || !npc.penalizedShots.insert(shotId).second) return;
    ScoreSystem::penalty(*this,scoreFeedback,human); dangerTime=human?1.5f:1.f; dangerHuman=human;
    soundEvents.push_back(human?SoundEvent::HumanPenalty:SoundEvent::Penalty);
    // Replace a struck NPC immediately, preserving the required active population.
    if (human) {
        auto replacement=createHuman(npc.zone,npc.home,npc.random,staticObjects,targets);
        replacement.penalizedShots=std::move(npc.penalizedShots); humans[index]=std::move(replacement);
    } else {
        auto replacement=createBird(npc.zone,npc.home,npc.random);
        replacement.penalizedShots=std::move(npc.penalizedShots); birds[index]=std::move(replacement);
    }
}
}
