#include "terrain/TerrainMapLoader.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "TestSuites.hpp"

namespace {

// --- Helpers (commented out until the Step 10.8 test groups use them: unused functions fail the CI build)
//
// // Wraps the given layer XML in a minimal orthogonal <map> element.
// std::string wrapInMap(const std::string& layersXml) {
//     return R"(<map version="1.10" orientation="orthogonal" width="30" height="20" tilewidth="32" tileheight="32">)" + layersXml + "</map>";
// }
//
// // Parses the wrapped map from a string. A false result means the test's own XML is broken, not the loader.
// bool loadTestMap(const char* scenario, const std::string& layersXml, tmx::Map& map) {
//     if (map.loadFromString(wrapInMap(layersXml), "")) {
//         return true;
//     }
//     std::cerr << "FAIL: " << scenario << ": the test map XML could not be parsed\n";
//     return false;
// }
//
// // Compares the loader's rectangles with the expected ones, in order.
// bool expectRectangles(const char* scenario, const std::vector<sf::FloatRect>& actual, const std::vector<sf::FloatRect>& expected) {
//     // These examples use whole pixels, which can be compared exactly as floats.
//     if (actual == expected) {
//         return true;
//     }
//
//     const auto printRectangles = [](const char* label, const std::vector<sf::FloatRect>& rectangles) {
//         std::cerr << "  " << label << ' ' << rectangles.size() << " rectangle(s)\n";
//         for (const sf::FloatRect& rectangle : rectangles) {
//             std::cerr << "    position (" << rectangle.position.x << ", " << rectangle.position.y << "), size (" << rectangle.size.x << ", " << rectangle.size.y << ")\n";
//         }
//     };
//     std::cerr << "FAIL: " << scenario << '\n';
//     printRectangles("expected", expected);
//     printRectangles("actual", actual);
//     return false;
// }
//
// // Expects the loader to throw std::invalid_argument whose message contains expectedMessage.
// bool expectLoadError(const char* scenario, const std::string& layersXml, const char* expectedMessage) {
//     tmx::Map map;
//     if (!loadTestMap(scenario, layersXml, map)) {
//         return false;
//     }
//
//     try {
//         // Only the exception matters here, so the returned rectangles are deliberately discarded.
//         static_cast<void>(loadTerrainRectangles(map));
//     } catch (const std::invalid_argument& error) {
//         const std::string_view actualMessage(error.what());
//         if (actualMessage.find(expectedMessage) == std::string_view::npos) {
//             std::cerr << "FAIL: " << scenario << " expected message containing '" << expectedMessage << "', got: " << error.what() << '\n';
//             return false;
//         }
//         return true;
//     } catch (const std::exception& error) {
//         std::cerr << "FAIL: " << scenario << " threw another exception: " << error.what() << '\n';
//         return false;
//     }
//
//     std::cerr << "FAIL: " << scenario << " did not throw std::invalid_argument\n";
//     return false;
// }

// --- Test groups

// Two rectangles, returned in their map order.
int testValidPlacement() {
    int failures = 0;
    return failures;
}

// A layer offset, applied exactly once.
int testLayerOffsets() {
    int failures = 0;
    return failures;
}

// A hidden object in a hidden layer is still solid.
int testHiddenObjects() {
    int failures = 0;
    return failures;
}

// One case per check: not a rectangle, tile object, rotated, zero size, non-finite. One case also checks the object ID and name in the message.
int testInvalidShapes() {
    int failures = 0;
    return failures;
}

// Missing, duplicate, wrong-type and nested Collision layers.
int testInvalidLayers() {
    int failures = 0;
    return failures;
}

} // namespace

int runTerrainMapLoaderTests() {
    int failures = 0;
    failures += testValidPlacement();
    failures += testLayerOffsets();
    failures += testHiddenObjects();
    failures += testInvalidShapes();
    failures += testInvalidLayers();
    return failures;
}
