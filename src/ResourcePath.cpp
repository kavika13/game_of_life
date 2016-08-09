#include "ResourcePath.hpp"

// Resource path doesn't exist with non-Apple bundles
std::string resourcePath() {
    return "";
}
