#include "TestSuites.hpp"

int main() {
    int failures = 0;
    failures += runTerrainCollisionTests();
    failures += runTerrainMapLoaderTests();
    // CTest uses the process exit code: zero passes, any nonzero value fails.
    return failures == 0 ? 0 : 1;
}
