#pragma once
#include <filesystem>
#include <string>
#include <vector>
namespace shooter {
std::string quoteCsv(const std::string& text);
std::vector<std::vector<std::string>> parseCsv(const std::string& text);
std::string utcTimestamp();
void replaceFile(const std::filesystem::path& temporary,const std::filesystem::path& destination);
void writeAtomicText(const std::filesystem::path& path,const std::string& contents);
}
