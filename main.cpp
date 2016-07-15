#include <unordered_set>
#include <cinttypes>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>

const unsigned int VIDEO_MODE_WIDTH = 1600, VIDEO_MODE_HEIGHT = 1200;
const unsigned int CELL_SHAPE_SIZE = 16;
const unsigned int CELL_GRID_SIZE = CELL_SHAPE_SIZE + 2;
const unsigned int FLASH_MESSAGE_OFFSET_X = 16;
const unsigned int FLASH_MESSAGE_OFFSET_Y = 12;
const unsigned int VIEWPORT_FIT_CELL_MARGIN = 6;
const sf::Color CELL_COLOR(150, 50, 250);
const sf::Color FLASH_MESSAGE_COLOR(sf::Color::Yellow);
const sf::Int32 MILLISECONDS_AUTO_FIT_FREQUENCY = 1000;
const sf::Int32 MILLISECONDS_PER_GENERATION = 1000 / 60;
const sf::Int32 SLOW_MOTION_MILLISECONDS_PER_GENERATION = 1000 / 2;
const sf::Int32 MILLISECONDS_DISPLAY_FLASH_MESSAGE = 1000 * 3;
const sf::Int32 MILLISECONDS_FADE_FLASH_MESSAGE = 1000 * 2 / 3;

// Helper to get the array size statically
template<typename TArrayType, std::size_t TArraySize>
constexpr std::size_t array_size(const TArrayType (&array)[TArraySize]) { return TArraySize; }

typedef std::int64_t CellDimension;
typedef std::tuple<CellDimension, CellDimension> CellOffset;
typedef std::tuple<CellOffset, CellOffset> CellRect;

class Cell {
public:
    Cell(CellDimension x = 0, CellDimension y = 0) : x_(x), y_(y) {}

    Cell offset(const CellOffset& offset) const {
        return Cell(x_ + std::get<0>(offset), y_ + std::get<1>(offset));
    }

    std::size_t get_hash_code() const {
        std::size_t result = 17;
        result = result * 23 + std::hash<CellDimension>()(x_);
        result = result * 23 + std::hash<CellDimension>()(y_);
        return result;
    }

    CellDimension X() const {
        return x_;
    }

    CellDimension Y() const {
        return y_;
    }

private:
    friend bool operator==(const Cell& lhs, const Cell& rhs);
    friend std::ostream& operator<<(std::ostream&, const Cell&);

    CellDimension x_;
    CellDimension y_;
};

bool operator==(const Cell& lhs, const Cell& rhs) {
    return lhs.x_ == rhs.x_ && lhs.y_ == rhs.y_;
}

std::ostream& operator<<(std::ostream& os, const Cell& cell) {
   return os << "<Cell: " << cell.x_ << ", " << cell.y_ << ">";
}

typedef std::unordered_set<Cell> CellContainer;

namespace std {
    template <> struct hash<Cell> {
        std::size_t operator()(const Cell& cell) const {
            return cell.get_hash_code();
        }
    };
}

std::ostream& operator<<(std::ostream& os, const CellContainer& cells) {
    for(const Cell& cell: cells) {
        os << cell << ", ";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const CellOffset& offset) {
    return os << "<" << std::get<0>(offset) << ", " << std::get<1>(offset) << ">";
}

CellRect get_bounds(const CellContainer& cells) {
    CellOffset lower_bound, upper_bound;

    for(const Cell& cell: cells) {
        std::get<0>(lower_bound) = std::min(cell.X(), std::get<0>(lower_bound));
        std::get<1>(lower_bound) = std::min(cell.Y(), std::get<1>(lower_bound));
        std::get<0>(upper_bound) = std::max(cell.X(), std::get<0>(upper_bound));
        std::get<1>(upper_bound) = std::max(cell.Y(), std::get<1>(upper_bound));
    }

    return CellRect(
        lower_bound,
        CellOffset(
            std::get<0>(upper_bound) - std::get<0>(lower_bound),
            std::get<1>(upper_bound) - std::get<1>(lower_bound))
    );
}

std::ostream& operator<<(std::ostream& os, const CellRect& rect) {
    return os << "<" << std::get<0>(rect) << ", " << std::get<1>(rect) << ">";
}

unsigned int count_bits_set(unsigned int value) {
    unsigned int count;

    for(count = 0; value; ++count) {
      value &= value - 1; // clear the least significant bit set
    }

    return count;
}

CellOffset neighbor_offsets[8] {
    {-1, 1},
    {0, 1},
    {1, 1},
    {-1, 0},
    {1, 0},
    {-1, -1},
    {0, -1},
    {1, -1},
};
const std::size_t potential_neighbor_count = array_size(neighbor_offsets);

const std::size_t lookup_size = 1 << potential_neighbor_count;
bool cell_spawn_lookup[lookup_size];
bool cell_die_lookup[lookup_size];

void initialize_rules() {
    for(unsigned int i = 0; i < lookup_size; ++i) {
        unsigned int count = count_bits_set(i);
        cell_spawn_lookup[i] = count == 3;
        cell_die_lookup[i] = count < 2 || count > 3;
    }
}

unsigned int neighbor_mask(const CellContainer& state, const Cell& cell) {
    unsigned int mask = 0;
    for(std::size_t i = 0; i < potential_neighbor_count; ++i) {
        mask |= state.find(cell.offset(neighbor_offsets[i])) == state.end() ? 0 : 1 << i;
    }
    return mask;
}

void advance(CellContainer& state) {
    CellContainer to_add;
    CellContainer to_remove;

    for(const Cell& cell: state) {
        unsigned int mask = neighbor_mask(state, cell);

        if(cell_die_lookup[mask]) {
            to_remove.insert(cell);
        }

        for(const CellOffset& offset: neighbor_offsets) {
            Cell neighbor = cell.offset(offset);

            if(state.find(neighbor) == state.end()) {
                unsigned int mask = neighbor_mask(state, neighbor);

                if(cell_spawn_lookup[mask]) {
                    to_add.insert(neighbor);
                }
            }
        }
    }

    for(const Cell& cell: to_add) {
        state.insert(cell);
    }
    for(const Cell& cell: to_remove) {
        state.erase(cell);
    }
}

// TODO: Does this template magic really make my life easier here? :)
template<std::size_t cells_count>
void add_shape(CellContainer& state, const Cell (&cells)[cells_count], CellOffset offset = {0, 0}) {
    for(std::size_t i = 0; i < array_size(cells); ++i) {
        state.insert(cells[i].offset(offset));
    }
}

void add_spinner(CellContainer& state, CellOffset offset = {0, 0}) {
    Cell cells[] {
        Cell(0, 0),
        Cell(1, 0),
        Cell(2, 0),
    };
    add_shape(state, cells, offset);
}

void add_block(CellContainer& state, CellOffset offset = {0, 0}) {
    Cell cells[] {
        Cell(0, 0),
        Cell(1, 0),
        Cell(0, 1),
        Cell(1, 1),
    };
    add_shape(state, cells, offset);
}

void load_state(const std::string& filename, CellContainer& state) {
    std::ifstream infile(filename);

    if(infile.fail()) {
        throw std::runtime_error(std::string("Failed to open file: ") + filename);
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

    if (!font.loadFromFile(filename)) {
        std::cerr << "Failed to load font: " << filename << "\n";
    }

    return font;
}

class FlashMessage : public sf::Text {
public:
    FlashMessage()
        : milliseconds_to_display_(MILLISECONDS_DISPLAY_FLASH_MESSAGE)
        , milliseconds_to_fade_(MILLISECONDS_FADE_FLASH_MESSAGE)
        , is_displayed_(false)
        , base_color_(sf::Text::getColor())
    {
    }

    FlashMessage(const sf::Font& font, unsigned int character_size=30)
        : sf::Text("", font, character_size)
        , milliseconds_to_display_(MILLISECONDS_DISPLAY_FLASH_MESSAGE)
        , milliseconds_to_fade_(MILLISECONDS_FADE_FLASH_MESSAGE)
        , is_displayed_(false)
        , base_color_(sf::Text::getColor())
    {
    }

    void setColor(const sf::Color& color) {
        base_color_ = color;
        sf::Text::setColor(color);
    }

    const sf::Color& getColor() const {
        return base_color_;
    }

    void Display(const std::string& message) {
        is_displayed_ = true;
        setString(message);
        sf::Text::setColor(base_color_);
        timer_.restart();
    }

    void Update() {
        sf::Int32 time_message_displayed = timer_.getElapsedTime().asMilliseconds();

        if(time_message_displayed < milliseconds_to_display_) {
            // No-op
        } else if(time_message_displayed < milliseconds_to_display_ + milliseconds_to_fade_) {
            sf::Color fade_color = sf::Color(base_color_);
            fade_color.a = 255 - static_cast<float>(time_message_displayed - milliseconds_to_display_) / milliseconds_to_fade_ * 255;
            sf::Text::setColor(fade_color);
        } else {
            is_displayed_ = false;
        }
    }

    void SetDisplayTime(sf::Int32 milliseconds_to_display) {
        milliseconds_to_display_ = milliseconds_to_display;
    }

    void SetFadeTime(sf::Int32 milliseconds_to_fade) {
        milliseconds_to_fade_ = milliseconds_to_fade;
    }

    bool is_displayed() const {
        return is_displayed_;
    }

private:
    sf::Int32 milliseconds_to_display_;
    sf::Int32 milliseconds_to_fade_;
    bool is_displayed_;
    sf::Color base_color_;
    sf::Clock timer_;
};

int main(int argc, char* argv[]) {
    initialize_rules();
    CellContainer state;

    load_state("input.txt", state);

    sf::RenderWindow window(sf::VideoMode(VIDEO_MODE_WIDTH, VIDEO_MODE_HEIGHT), "Conway's Game of Life");

    sf::Clock generation_timer;

    sf::CircleShape cell_shape(CELL_SHAPE_SIZE / 2); // Takes a radius
    cell_shape.setFillColor(CELL_COLOR);

    sf::Font flash_message_font = load_font("Inconsolata-Regular.ttf");
    FlashMessage flash_message(flash_message_font);
    flash_message.setPosition(FLASH_MESSAGE_OFFSET_X, FLASH_MESSAGE_OFFSET_Y);
    flash_message.setColor(FLASH_MESSAGE_COLOR);

    // Center origin on the screen, and make positive-Y axis point up instead of down
    sf::Transform global_transform;
    global_transform.translate(VIDEO_MODE_WIDTH / 2, VIDEO_MODE_HEIGHT / 2);
    global_transform.scale(1.0f, -1.0f);

    sf::Transform viewport_transform;
    float viewportX, viewportY, viewportZoom;

    sf::Clock auto_fit_timer;

    const auto set_viewport = [&]() {
        viewport_transform = sf::Transform()
            .scale(viewportZoom, viewportZoom)
            .translate(viewportX, viewportY);
    };

    const auto reset_viewport = [&]() {
        viewportX = 0.0f, viewportY = 0.0f, viewportZoom = 1.0f;
        set_viewport();
    };

    const auto fit_viewport = [&]() {
        CellRect rect = get_bounds(state);
        CellOffset lower_bound = std::get<0>(rect), dimensions = std::get<1>(rect);
        CellDimension cells_width = std::get<0>(dimensions) + 1;
        CellDimension cells_height = std::get<1>(dimensions) + 1;

        viewportZoom = std::min(
            VIDEO_MODE_WIDTH / CELL_GRID_SIZE / static_cast<float>(cells_width + VIEWPORT_FIT_CELL_MARGIN),
            VIDEO_MODE_HEIGHT / CELL_GRID_SIZE / static_cast<float>(cells_height + VIEWPORT_FIT_CELL_MARGIN));
        viewportX = -(VIDEO_MODE_WIDTH / 2.0f / viewportZoom) - std::get<0>(lower_bound) * CELL_GRID_SIZE;
        viewportY = -(VIDEO_MODE_HEIGHT / 2.0f / viewportZoom) - std::get<1>(lower_bound) * CELL_GRID_SIZE;

        float cell_width_delta = VIDEO_MODE_WIDTH - cells_width * CELL_GRID_SIZE * viewportZoom;
        float cell_height_delta = VIDEO_MODE_HEIGHT - cells_height * CELL_GRID_SIZE * viewportZoom;
        viewportX += cell_width_delta / 2.0f / viewportZoom;
        viewportY += cell_height_delta / 2.0f / viewportZoom;

        set_viewport();
    };

    reset_viewport();

    bool is_paused = true;
    bool is_in_slow_motion = false;
    bool is_auto_fit_enabled = false;

    const std::string instructions_message(
        "Hit Space to toggle pause.\n" \
        "Hit S to toggle slow motion.\n" \
        "Hit Arrow Keys to shift the viewport.\n" \
        "Hold the Shift key to move the viewport more quickly.\n" \
        "Hit Z to zoom the viewport.\n" \
        "Hit X to zoom out the viewport.\n" \
        "Hit F to fit the viewport to the alive cells.\n" \
        "Hit A to toggle viewport auto-fit.\n" \
        "Hit R to reset the viewport.\n" \
        "Hit ? or H to display these instructions.\n" \
        "Hit Escape or Q to quit.");
    flash_message.Display("Simulation begins paused.\n\n" + instructions_message);

    while(window.isOpen()) {
        sf::Event event;

        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                window.close();
            }

            if(event.type == sf::Event::KeyReleased) {
                if(event.key.code == sf::Keyboard::H || (event.key.code == sf::Keyboard::Slash && event.key.shift)) {
                    flash_message.Display(instructions_message);
                }

                if(event.key.code == sf::Keyboard::Space) {
                    is_paused = !is_paused;
                    flash_message.Display(is_paused ? "Paused" : "Unpaused");

                    if(!is_paused) {
                        generation_timer.restart();
                    }
                }

                if(event.key.code == sf::Keyboard::S) {
                    is_in_slow_motion = !is_in_slow_motion;
                    flash_message.Display(is_in_slow_motion ? "Slow motion activated" : "Normal speed activated");
                }

                if(event.key.code == sf::Keyboard::R) {
                    reset_viewport();
                }

                if(event.key.code == sf::Keyboard::F) {
                    fit_viewport();
                }

                if(event.key.code == sf::Keyboard::A) {
                    is_auto_fit_enabled = !is_auto_fit_enabled;
                    flash_message.Display(is_auto_fit_enabled ? "Auto-fit enabled" : "Auto-fit disabled");

                    if(is_auto_fit_enabled) {
                        fit_viewport();
                        auto_fit_timer.restart();
                    }
                }

                if(event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::Q) {
                    window.close();
                }
            }

            if(event.type == sf::Event::KeyPressed) {
                bool is_viewport_changed(false);
                const float distance = (event.key.shift ? CELL_GRID_SIZE * 10 : CELL_GRID_SIZE)
                    / (viewportZoom < 1.0f ? viewportZoom : 1.0f);

                if(event.key.code == sf::Keyboard::Left) {
                    is_viewport_changed = true;
                    viewportX += distance;
                }

                if(event.key.code == sf::Keyboard::Right) {
                    is_viewport_changed = true;
                    viewportX -= distance;
                }

                if(event.key.code == sf::Keyboard::Down) {
                    is_viewport_changed = true;
                    viewportY += distance;
                }

                if(event.key.code == sf::Keyboard::Up) {
                    is_viewport_changed = true;
                    viewportY -= distance;
                }

                if(event.key.code == sf::Keyboard::Z) {
                    is_viewport_changed = true;
                    viewportZoom *= 2.0f;
                }

                if(event.key.code == sf::Keyboard::X) {
                    is_viewport_changed = true;
                    viewportZoom /= 2.0f;
                }

                if(is_viewport_changed) {
                    if(is_auto_fit_enabled) {
                        flash_message.Display("Auto-fit disabled");
                        is_auto_fit_enabled = false;
                    }

                    set_viewport();
                }
            }
        }

        if(is_auto_fit_enabled) {
            if(auto_fit_timer.getElapsedTime().asMilliseconds() >= MILLISECONDS_AUTO_FIT_FREQUENCY) {
                fit_viewport();
                auto_fit_timer.restart();
            }
        }

        const sf::Int32 minimum_frame_time = is_in_slow_motion
            ? SLOW_MOTION_MILLISECONDS_PER_GENERATION
            : MILLISECONDS_PER_GENERATION;

        if(!is_paused) {
            if(generation_timer.getElapsedTime().asMilliseconds() >= minimum_frame_time) {
                advance(state);
                generation_timer.restart();
            }
        }

        window.clear(sf::Color::Black);

        for(const Cell& cell: state) {
            cell_shape.setPosition(
                cell.X() * CELL_GRID_SIZE,
                cell.Y() * CELL_GRID_SIZE);
            window.draw(cell_shape, global_transform * viewport_transform);
        }

        if(flash_message.is_displayed()) {
            flash_message.Update();
            window.draw(flash_message);
        }

        window.display();
    }

    return 0;
}
