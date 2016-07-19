#include <iostream>
#include <fstream>
#include <sstream>
#include "resources.hpp"
#include "ResourcePath.hpp"
#include "arraysize.hpp"

void load_state(const std::string& filename, CellContainer& state) {
    std::ifstream infile(resourcePath() + filename);

    if(infile.fail()) {
        throw std::runtime_error(std::string("Failed to open file: ") + resourcePath() + filename);
    }

    std::string line;
    int line_number = 1;

    const auto matches_comment = [](const std::string& line) {
        const std::string comments[] = {"//", "#", ";"};
        return std::any_of(
            comments,
            comments + array_size(comments),
            [&](const std::string& prefix) {
                return std::equal(prefix.begin(), prefix.end(), line.begin());
            });
    };

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        CellDimension x, y;
        char c;

        if(line.empty() || matches_comment(line)) {
            continue;
        }

        iss.ignore(); // Skip opening parentheses

        if (!(iss >> x >> c >> y) && (c == ',')) {
            throw std::runtime_error(std::string("Failed to parse file at line: ") + std::to_string(line_number));
        }

        state.insert(Cell(x, y));
        ++line_number;
    }
}

sf::Font load_font(const std::string& filename) {
    sf::Font font;

    if (!font.loadFromFile(resourcePath() + filename)) {
        std::cerr << "Failed to load font: " << resourcePath() + filename << "\n";
    }

    return font;
}
