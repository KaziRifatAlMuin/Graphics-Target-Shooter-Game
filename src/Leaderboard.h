#pragma once
#include "GameMode.h"
#include <filesystem>
#include <vector>
namespace shooter {
struct LeaderboardRecord {
    std::string name;
    GameMode mode=GameMode::Challenge;
    RunStats stats;
    std::string lastUpdated;
};
class Leaderboard {
public:
    explicit Leaderboard(std::filesystem::path path);
    void load(); // Creates only a missing file. Existing valid records are retained.
    bool submit(const EligibleResult& result); // True only for a new personal best.
    std::vector<LeaderboardRecord> sorted(GameMode mode) const;
    int rank(const std::string& name,GameMode mode) const;
    const std::vector<LeaderboardRecord>& records() const { return entries; }
    static bool better(const RunStats& candidate,const RunStats& previous);
    static bool eligible(const EligibleResult& result);
    const std::string& warning() const { return loadWarning; }
private:
    std::filesystem::path file;
    std::vector<LeaderboardRecord> entries;
    std::string loadWarning;
    void save(const std::vector<LeaderboardRecord>& records) const;
};
}
