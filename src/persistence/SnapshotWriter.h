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
    // Start a background worker targeting the supplied calculation file.
    explicit SnapshotWriter(std::filesystem::path path);
    // Finish pending work and join the worker before destroying its shared state.
    ~SnapshotWriter();
    // Queue the newest owned scene snapshot, replacing any older pending snapshot.
    void submit(std::vector<SceneObject> snapshot);
    // Wait for queued writes and propagate a saved write error to the caller.
    void flush();
private:
    std::filesystem::path destination;
    std::mutex mutex;
    std::condition_variable wake,drained;
    std::optional<std::vector<SceneObject>> pending;
    bool stopping=false,busy=false;
    std::exception_ptr failure;
    std::thread worker;
    // Worker loop: wait, take a snapshot, write it, and notify waiting callers.
    void run();
};
}
