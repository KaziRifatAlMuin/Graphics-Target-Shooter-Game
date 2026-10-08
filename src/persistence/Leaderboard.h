#pragma once
#include "gameplay/GameMode.h"
#include <filesystem>
#include <vector>
namespace shooter {
// Store one player's best result for one mode, with its last update timestamp.
struct LeaderboardRecord {
    std::string name;
    GameMode mode=GameMode::Challenge;
    RunStats stats;
    std::string lastUpdated;
};
// Manage persistent best results and derive sorted rows/ranks for the interface.
class Leaderboard {
public:
    // Remember the leaderboard path without reading or overwriting it yet.
    explicit Leaderboard(std::filesystem::path path);
    // Read valid CSV records, keep the best duplicate name/mode entry, and report skipped invalid rows.
    void load(); // Creates only a missing file. Existing valid records are retained.
    // Persist only eligible personal bests; update memory after the file save succeeds.
    bool submit(const EligibleResult& result); // True only for a new personal best.
    // Filter one mode and sort by score, then time, then name for deterministic ties.
    std::vector<LeaderboardRecord> sorted(GameMode mode) const;
    // Return a player's one-based position in the selected mode, or zero if absent.
    int rank(const std::string& name,GameMode mode) const;
    const std::vector<LeaderboardRecord>& records() const { return entries; }
    // Higher score wins; equal scores are ordered by shorter elapsed time.
    static bool better(const RunStats& candidate,const RunStats& previous);
    // Accept completed Challenge progress or a full timed Free session with valid name/statistics.
    static bool eligible(const EligibleResult& result);
    const std::string& warning() const { return loadWarning; }
private:
    std::filesystem::path file;
    std::vector<LeaderboardRecord> entries;
    std::string loadWarning;
    // Serialize records with sufficient time precision and replace the CSV only after writing succeeds.
    void save(const std::vector<LeaderboardRecord>& records) const;
};
}
