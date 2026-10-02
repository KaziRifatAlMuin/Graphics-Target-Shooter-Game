#pragma once
#include "persistence/CsvLogger.h"
#include <condition_variable>
#include <exception>
#include <mutex>
#include <optional>
#include <thread>
namespace shooter {
// A snapshot owns its exact observed transforms. Slow disks never block the render loop.
// Pending snapshots coalesce to the newest one; shutdown always flushes the final scene.
class SnapshotWriter {
public:
    explicit SnapshotWriter(std::filesystem::path path);
    ~SnapshotWriter();
    void submit(std::vector<SceneObject> snapshot);
    void flush();
private:
    std::filesystem::path destination;
    std::mutex mutex;
    std::condition_variable wake,drained;
    std::optional<std::vector<SceneObject>> pending;
    bool stopping=false,busy=false;
    std::exception_ptr failure;
    std::thread worker;
    void run();
};
}
