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
    cachedAimFrame=-1;
    staticObjects=createLevelWorld(levels.config); targets=levels.config.targets;
    for (auto& target:targets) updateTarget(target,0,0);
    player.position={0,1.7f,-5}; player.yaw=-90; player.pitch=3.8f;
    freeCamera=Camera{}; cameraMode=1; projectiles.clear(); debris.clear(); soundEvents.clear();
    scoreFeedback.clear(); cooldown=recoil=feedbackTime=dangerTime=0; lastRing=-1;
    soundEvents.push_back(SoundEvent::Start);
    birds.clear(); humans.clear();
    const auto& c=levels.config;
    const int birdCount=c.birdsPerTarget?int(targets.size())*std::clamp(c.birdsPerTarget,0,10):std::min(c.birds,int(targets.size())*10);
    for (int i=0;i<birdCount;++i) {
        const std::size_t zone=std::size_t(i)%targets.size();
        birds.push_back(createBird(zone,targets[zone].base,c.seed+unsigned(i)*193+47));
    }
    for (std::size_t zone=0;zone<targets.size();++zone) for (int i=0;i<std::clamp(c.humansPerTarget,0,10);++i)
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
void Game::updateNpcDeaths(float dt) {
    for (auto& b:birds) if (b.dying) updateBird(b,dt,staticObjects);
    for (auto& h:humans) if (h.dying) updateHuman(h,dt,staticObjects,targets);
}
void Game::updateNpcs(float dt) {
    // Keep survivors alive. Reassign only when their target is permanently eliminated.
    // Counts are separate for birds/humans; excess survivors wait in their old zone.
    auto redistribute=[&](auto& population, auto relocate) {
        std::vector<int> counts(targets.size(),0);
        for(const auto& npc:population)
            if(npc.active && !targets[npc.zone].eliminated) ++counts[npc.zone];
        for(auto& npc:population) {
            if(!npc.active || !targets[npc.zone].eliminated) continue;
            std::vector<std::size_t> available;
            for(std::size_t zone=0;zone<targets.size();++zone)
                if(!targets[zone].eliminated && counts[zone]<10) available.push_back(zone);
            if(available.empty()) continue; // No killing, respawning or overfilling.
            const auto choice=std::min(std::size_t(npcRandom(npc)*available.size()),available.size()-1);
            npc.zone=available[choice]; ++counts[npc.zone];
            npc.home=targets[npc.zone].base;
            relocate(npc); // Reposition safely without changing identity/life/penalty history.
        }
    };
    redistribute(birds,[&](Bird& b) {
        b.position=npcDestination(b,true); b.destination=npcDestination(b,true); b.pause=0;
    });
    redistribute(humans,[&](Human& h) {
        const auto placed=createHuman(h.zone,h.home,h.random,staticObjects,targets);
        h.position=placed.position; h.destination=placed.destination; h.random=placed.random; h.pause=0;
    });
    for(auto& b:birds) if(b.active) updateBird(b,dt,staticObjects);
    for(auto& h:humans) if(h.active) updateHuman(h,dt,staticObjects,targets);
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
    cachedAimFrame=-1;
    npc.active=false;
    npc.dying=true;
    npc.dead=false;
    npc.deathTime=0;
    if (human) {
        auto& h=humans.at(index);
        soundEvents.push_back(SoundEvent::HumanDie);
        soundEvents.push_back(SoundEvent::HumanPenalty);
        spawnBlood(h.position+Vec3{0,h.height*.65f,0},24);
    } else {
        auto& b=birds.at(index);
        soundEvents.push_back(SoundEvent::BirdDie);
        soundEvents.push_back(SoundEvent::Penalty);
        b.deathVelocity=Vec3{(npcRandom(b)-.5f)*2.4f,1.2f+npcRandom(b)*1.8f,(npcRandom(b)-.5f)*2.4f};
        spawnBlood(b.position,14);
    }
}
}
