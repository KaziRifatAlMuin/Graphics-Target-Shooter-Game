#include "persistence/SnapshotWriter.h"
namespace shooter {
// Start a background writer so calculation CSV output does not run inside the rendering loop.
SnapshotWriter::SnapshotWriter(std::filesystem::path path):destination(std::move(path)),worker([this] { run(); }) {}
// Signal shutdown and wait for the worker to finish its final pending snapshot.
SnapshotWriter::~SnapshotWriter() {
    { std::lock_guard<std::mutex> guard(mutex); stopping=true; }
    wake.notify_one(); worker.join();
}
// Replace the pending snapshot under a lock, keeping the latest scene when disk writes lag.
void SnapshotWriter::submit(std::vector<SceneObject> snapshot) {
    { std::lock_guard<std::mutex> guard(mutex); pending=std::move(snapshot); }
    wake.notify_one();
}
// Wait until no write is pending or active, then report any stored write error.
void SnapshotWriter::flush() {
    std::unique_lock<std::mutex> guard(mutex);
    drained.wait(guard,[this] { return !pending && !busy; });
    if (failure) std::rethrow_exception(failure);
}
// Wait for work, take ownership under the lock, write outside the lock, and signal completion.
void SnapshotWriter::run() {
    for (;;) {
        std::vector<SceneObject> snapshot;
        {
            std::unique_lock<std::mutex> guard(mutex);
            wake.wait(guard,[this] { return stopping || pending.has_value(); });
            if (!pending && stopping) return;
            snapshot=std::move(*pending); pending.reset(); busy=true;
        }
        std::exception_ptr error;
        try { writeCalculations(snapshot,destination); } catch (...) { error=std::current_exception(); }
        {
            std::lock_guard<std::mutex> guard(mutex);
            failure=error; busy=false;
        }
        drained.notify_all();
    }
}
}
