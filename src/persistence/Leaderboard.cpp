#include "persistence/Leaderboard.h"
#include "persistence/CsvFile.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>
namespace shooter {
namespace {
const std::string header="Name,Mode,BestScore,LevelsCleared,TargetsDestroyed,Bullseyes,BirdKills,HumanKills,BestTimeSeconds,LastUpdated";
// Parse a complete integer field; reject trailing characters instead of accepting a partial number.
int integer(const std::string& s) { std::size_t n=0; int v=std::stoi(s,&n); if (n!=s.size()) throw std::invalid_argument("Integer"); return v; }
// Reject impossible counts, negative times, and non-finite elapsed values.
bool validStats(const RunStats& s) {
    return s.levelsCleared>=0 && s.levelsCleared<=7 && s.destroyed>=0 && s.bullseyes>=0 &&
        s.bullseyes<=s.destroyed && s.birdHits>=0 && s.humanHits>=0 && std::isfinite(s.elapsed) && s.elapsed>=0;
}
// Require a nonblank, length-limited name without control characters.
bool validName(const std::string& s) {
    return !s.empty() && s.size()<=128 && s.find_first_not_of(' ')!=std::string::npos &&
        std::none_of(s.begin(),s.end(),[](unsigned char c) { return c<32 || c==127; });
}
}
// Remember the leaderboard path without reading or overwriting it yet.
Leaderboard::Leaderboard(std::filesystem::path path):file(std::move(path)) {}
// Higher score wins; equal scores are ordered by shorter elapsed time.
bool Leaderboard::better(const RunStats& a,const RunStats& b) {
    return a.score>b.score || (a.score==b.score && a.elapsed<b.elapsed);
}
// Accept completed Challenge progress or a full timed Free session with valid name/statistics.
bool Leaderboard::eligible(const EligibleResult& r) {
    if (!validName(r.name) || !validStats(r.stats)) return false;
    if (r.mode==GameMode::Challenge) return r.stats.levelsCleared>0;
    return r.mode==GameMode::Free && r.stats.levelsCleared==0 && std::abs(r.stats.elapsed-freeSessionSeconds)<1e-6;
}
// Read valid CSV records, keep the best duplicate name/mode entry, and report skipped invalid rows.
void Leaderboard::load() {
    loadWarning.clear();
    if (!std::filesystem::exists(file)) { save({}); entries.clear(); return; }
    std::ifstream in(file,std::ios::binary);
    if (!in) throw std::runtime_error("Cannot read leaderboard: "+file.string());
    const std::string text((std::istreambuf_iterator<char>(in)),{});
    const auto rows=parseCsv(text);
    if (rows.empty() || rows.front()!=parseCsv(header).front()) throw std::runtime_error("Unsupported leaderboard header; existing file preserved.");
    std::vector<LeaderboardRecord> valid; int skipped=0;
    for (std::size_t i=1;i<rows.size();++i) {
        try {
            const auto& v=rows[i];
            if (v.size()!=10 || !validName(v[0]) || (v[1]!="Free" && v[1]!="Challenge")) throw std::invalid_argument("Record");
            LeaderboardRecord r; r.name=v[0]; r.mode=v[1]=="Free"?GameMode::Free:GameMode::Challenge;
            r.stats.score=integer(v[2]); r.stats.levelsCleared=integer(v[3]); r.stats.destroyed=integer(v[4]);
            r.stats.bullseyes=integer(v[5]); r.stats.birdHits=integer(v[6]); r.stats.humanHits=integer(v[7]);
            std::istringstream time(v[8]); time.imbue(std::locale::classic());
            if (!(time>>r.stats.elapsed) || time.peek()!=std::char_traits<char>::eof() || !validStats(r.stats)) throw std::invalid_argument("Stats");
            r.lastUpdated=v[9];
            auto previous=std::find_if(valid.begin(),valid.end(),[&](const LeaderboardRecord& e) { return e.name==r.name && e.mode==r.mode; });
            if (previous==valid.end()) valid.push_back(r);
            else if (better(r.stats,previous->stats)) *previous=r;
        } catch (const std::invalid_argument&) { ++skipped; }
          catch (const std::out_of_range&) { ++skipped; }
    }
    entries=std::move(valid);
    if (skipped) loadWarning="Skipped "+std::to_string(skipped)+" invalid leaderboard rows; valid records retained.";
}
// Serialize records with sufficient time precision and replace the CSV only after writing succeeds.
void Leaderboard::save(const std::vector<LeaderboardRecord>& records) const {
    std::ostringstream out; out.imbue(std::locale::classic());
    out<<header<<'\n'<<std::setprecision(17);
    for (const auto& r:records) {
        const auto& s=r.stats;
        out<<quoteCsv(r.name)<<','<<modeName(r.mode)<<','<<s.score<<','<<s.levelsCleared<<','<<s.destroyed<<','
            <<s.bullseyes<<','<<s.birdHits<<','<<s.humanHits<<','<<s.elapsed<<','<<quoteCsv(r.lastUpdated)<<'\n';
    }
    writeAtomicText(file,out.str());
}
// Persist only eligible personal bests; update memory after the file save succeeds.
bool Leaderboard::submit(const EligibleResult& result) {
    if (!eligible(result)) return false;
    // Reload before merging so a later session cannot overwrite another valid saved row.
    load();
    auto next=entries;
    auto found=std::find_if(next.begin(),next.end(),[&](const LeaderboardRecord& e) { return e.name==result.name && e.mode==result.mode; });
    if (found!=next.end() && !better(result.stats,found->stats)) return false;
    LeaderboardRecord record{result.name,result.mode,result.stats,utcTimestamp()};
    if (found==next.end()) next.push_back(record); else *found=record;
    save(next); entries=std::move(next); return true;
}
// Filter one mode and sort by score, then time, then name for deterministic ties.
std::vector<LeaderboardRecord> Leaderboard::sorted(GameMode mode) const {
    std::vector<LeaderboardRecord> rows;
    for (const auto& e:entries) if (e.mode==mode) rows.push_back(e);
    std::sort(rows.begin(),rows.end(),[](const LeaderboardRecord& a,const LeaderboardRecord& b) {
        if (better(a.stats,b.stats)) return true;
        if (better(b.stats,a.stats)) return false;
        return a.name<b.name; // Deterministic ordering for an exact score/time tie.
    });
    return rows;
}
// Return a player's one-based position in the selected mode, or zero if absent.
int Leaderboard::rank(const std::string& name,GameMode mode) const {
    const auto rows=sorted(mode);
    for (std::size_t i=0;i<rows.size();++i) if (rows[i].name==name) return int(i)+1;
    return 0;
}
}
