#include "persistence/SnapshotWriter.h"
namespace shooter {
SnapshotWriter::SnapshotWriter(std::filesystem::path path):destination(std::move(path)),worker([this] { run(); }) {}
SnapshotWriter::~SnapshotWriter() {
    { std::lock_guard<std::mutex> guard(mutex); stopping=true; }
    wake.notify_one(); worker.join();
}
void SnapshotWriter::submit(std::vector<SceneObject> snapshot) {
    { std::lock_guard<std::mutex> guard(mutex); pending=std::move(snapshot); }
    wake.notify_one();
}
void SnapshotWriter::flush() {
    std::unique_lock<std::mutex> guard(mutex);
    drained.wait(guard,[this] { return !pending && !busy; });
    if (failure) std::rethrow_exception(failure);
}
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
