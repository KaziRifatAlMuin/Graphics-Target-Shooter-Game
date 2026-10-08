#pragma once
#include <filesystem>
#include <string>
#include <vector>
namespace shooter {
// Surround a CSV field with quotes and double embedded quotes so commas/newlines remain part of the field.
std::string quoteCsv(const std::string& text);
// Parse quoted CSV fields, escaped quotes, and line endings; reject malformed quoting.
std::vector<std::vector<std::string>> parseCsv(const std::string& text);
// Format the current UTC time while locking access to the shared gmtime conversion buffer.
std::string utcTimestamp();
// Replace the destination with the completed temporary file using the platform's file operation.
void replaceFile(const std::filesystem::path& temporary,const std::filesystem::path& destination);
// Write and close a temporary file before replacement, avoiding a partially written destination.
void writeAtomicText(const std::filesystem::path& path,const std::string& contents);
}
