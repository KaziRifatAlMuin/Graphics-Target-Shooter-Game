#pragma once
#include "SceneObject.h"
#include <filesystem>
#include <map>

namespace shooter {
void writeCalculations(const std::vector<SceneObject>& objects, const std::filesystem::path& path);
// Exact last-observed transient transforms survive between periodic disk writes.
// Cleared at process startup; never imports stale CSV.
class CsvLogger {
public:
    void observe(const std::vector<SceneObject>& objects, double time, bool night);
    void save(const std::filesystem::path& path) const;
private:
    std::vector<SceneObject> current;
    std::map<std::string,SceneObject> observed;
};
}
