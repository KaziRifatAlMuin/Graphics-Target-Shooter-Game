#include "world/LevelWorld.h"
#include "world/Arena.h"
#include "world/Cargo.h"
#include "core/Collision.h"
#include "world/Environment.h"
#include "world/Lighting.h"
#include <queue>
#include <stdexcept>
namespace shooter {
bool clearSightline(Vec3 from, Vec3 to, const std::vector<SceneObject>& objects) {
    const auto direction=normalize(to-from); const float distance=length(to-from);
    for (const auto& o:objects) if (intersectCube(from,direction,o.transform,distance)<distance) return false;
    return true;
}
void validateLevelWorld(const LevelConfig& c, const std::vector<SceneObject>& objects) {
    const Vec3 spawn{0,1.7f,-5};
    if (!c.playerMovement) {
        for (const auto& original:c.targets) for (int sample=0;sample<32;++sample) {
            auto t=original; updateTarget(t,sample*.43f,0);
            if (length(t.position-spawn)>68 || !clearSightline(spawn,t.position,objects))
                throw std::runtime_error("Fixed-player level has an obstructed/out-of-range target.");
        }
        return;
    }
    // Flood fill the walkable meter grid from spawn; no diagonal corner cutting.
    constexpr int width=51,depth=91;
    std::vector<bool> open(width*depth),visited(width*depth);
    auto position=[](int x,int z) { return Vec3{float(x-25),1.7f,float(-5-z)}; };
    for (int z=0;z<depth;++z) for (int x=0;x<width;++x) open[z*width+x]=canStandAt(position(x,z),objects);
    std::queue<int> pending; pending.push(25); visited[25]=true;
    while (!pending.empty()) {
        const int i=pending.front(); pending.pop();
        const int x=i%width,z=i/width;
        for (auto step:std::array<std::array<int,2>,4>{{{1,0},{-1,0},{0,1},{0,-1}}}) {
            const int nx=x+step[0],nz=z+step[1];
            if (nx<0||nx>=width||nz<0||nz>=depth) continue;
            const int next=nz*width+nx;
            if (visited[next]||!open[next]) continue;
            const Vec3 a=position(x,z),b=position(nx,nz);
            if (!canStandAt((a+b)*.5f,objects)) continue;
            visited[next]=true; pending.push(next);
        }
    }
    for (std::size_t index=0;index<c.targets.size();++index) {
        const auto& t=c.targets[index]; bool accessible=false;
        for (int z=0;z<depth && !accessible;++z) for (int x=0;x<width && !accessible;++x) {
            if (!visited[z*width+x]) continue;
            const auto p=position(x,z);
            if (p.z<t.base.z+2 || length(p-t.base)>10) continue;
            accessible=clearSightline(p,t.base,objects);
        }
        if (!accessible) throw std::runtime_error("No reachable firing position for target "+std::to_string(index+1));
    }
}
std::vector<SceneObject> createLevelWorld(const LevelConfig& c) {
    auto objects=createArena();
    for (const auto& group:{createLightFixtures(),createEquipmentDisplay()}) objects.insert(objects.end(),group.begin(),group.end());
    // Retry the procedural side clusters if validation ever fails for a new seed.
    for (unsigned attempt=0;attempt<4;++attempt) {
        auto trial=objects; auto settings=c; settings.seed+=attempt*7919;
        const auto cargo=generateChallengeCargo(settings); trial.insert(trial.end(),cargo.begin(),cargo.end());
        try { validateLevelWorld(c,trial); return trial; }
        catch (const std::runtime_error&) { if (attempt==3) throw; }
    }
    return objects;
}
}
