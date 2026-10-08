#include "persistence/CsvFile.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <mutex>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
namespace shooter {
// Surround a CSV field with quotes and double embedded quotes so commas/newlines remain part of the field.
std::string quoteCsv(const std::string& text) {
    std::string out="\"";
    for (char c:text) { if (c=='\"') out+='\"'; out+=c; }
    return out+'\"';
}
// Parse quoted CSV fields, escaped quotes, and line endings; reject malformed quoting.
std::vector<std::vector<std::string>> parseCsv(const std::string& text) {
    std::vector<std::vector<std::string>> rows;
    std::vector<std::string> row; std::string field;
    bool quoted=false,closed=false;
    const std::size_t start=text.compare(0,3,"\xEF\xBB\xBF")==0?3:0;
    for (std::size_t i=start;i<text.size();++i) {
        const char c=text[i];
        if (quoted) {
            if (c=='\"' && i+1<text.size() && text[i+1]=='\"') { field+='\"'; ++i; }
            else if (c=='\"') { quoted=false; closed=true; }
            else field+=c;
        } else if (c=='\"' && field.empty() && !closed) quoted=true;
        else if (c==',' || c=='\n' || c=='\r') {
            row.push_back(field); field.clear(); closed=false;
            if (c!=',') {
                if (c=='\r' && i+1<text.size() && text[i+1]=='\n') ++i;
                if (row.size()!=1 || !row[0].empty()) rows.push_back(row);
                row.clear();
            }
        } else {
            if (closed || c=='\"') throw std::runtime_error("Malformed CSV quoting.");
            field+=c;
        }
    }
    if (quoted) throw std::runtime_error("Unclosed CSV quote.");
    if (!field.empty() || !row.empty() || closed) { row.push_back(field); rows.push_back(row); }
    return rows;
}
// Format the current UTC time while locking access to the shared gmtime conversion buffer.
std::string utcTimestamp() {
    static std::mutex timestampMutex;
    std::lock_guard<std::mutex> guard(timestampMutex);
    const auto now=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::ostringstream out; out<<std::put_time(std::gmtime(&now),"%Y-%m-%dT%H:%M:%SZ"); return out.str();
}
// Replace the destination with the completed temporary file using the platform's file operation.
void replaceFile(const std::filesystem::path& temporary,const std::filesystem::path& destination) {
#ifdef _WIN32
    if (!MoveFileExW(temporary.c_str(),destination.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))
        throw std::runtime_error("Cannot replace CSV (close any application locking it): "+destination.string());
#else
    std::filesystem::rename(temporary,destination);
#endif
}
// Write and close a temporary file before replacement, avoiding a partially written destination.
void writeAtomicText(const std::filesystem::path& path,const std::string& contents) {
    auto temporary=path; temporary+=".tmp";
    std::ofstream out(temporary,std::ios::binary|std::ios::trunc);
    if (!out) throw std::runtime_error("Cannot write CSV: "+path.string());
    out<<contents; out.close();
    if (!out) throw std::runtime_error("Cannot finish CSV: "+path.string());
    replaceFile(temporary,path);
}
}
