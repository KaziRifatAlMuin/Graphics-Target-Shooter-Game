#include "core/Application.h"

// Program entry point: pass command-line arguments to the application and return its exit status.
int main(int argc, char** argv) {
    return shooter::Application{}.run(argc, argv);
}
