#include <unordered_set>
#include <cinttypes>
#include <iostream>
#include <map>
#include <SFML/Graphics.hpp>

const unsigned int VIDEO_MODE_WIDTH = 1600, VIDEO_MODE_HEIGHT = 1200;
const unsigned int CELL_SHAPE_SIZE = 16;
const unsigned int CELL_GRID_SIZE = CELL_SHAPE_SIZE + 2;
const unsigned int FLASH_MESSAGE_OFFSET_X = 16;
const unsigned int FLASH_MESSAGE_OFFSET_Y = 12;
const sf::Color CELL_COLOR(150, 50, 250);
const sf::Color FLASH_MESSAGE_COLOR(sf::Color::Yellow);
const sf::Int32 MILLISECONDS_PER_GENERATION = 1000 / 60;
const sf::Int32 SLOW_MOTION_MILLISECONDS_PER_GENERATION = 1000 / 2;
const sf::Int32 MILLISECONDS_DISPLAY_FLASH_MESSAGE = 1000 * 3;
const sf::Int32 MILLISECONDS_FADE_FLASH_MESSAGE = 1000 * 2 / 3;

// Helper to get the array size statically
template<typename TArrayType, std::size_t TArraySize>
constexpr std::size_t array_size(const TArrayType (&array)[TArraySize]) { return TArraySize; }

typedef std::int64_t CellDimension;
typedef std::tuple<CellDimension, CellDimension> CellOffset;

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

    add_spinner(state);

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

    bool is_paused = true;
    bool is_in_slow_motion = false;

    const std::string instructions_message(
        "Hit Space to toggle pause.\n" \
        "Hit S to toggle slow motion.\n" \
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

                if(event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::Q) {
                    window.close();
                }
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

        if(flash_message.is_displayed()) {
            flash_message.Update();
            window.draw(flash_message);
        }

        for(const Cell& cell: state) {
            cell_shape.setPosition(
                cell.X() * CELL_GRID_SIZE,
                cell.Y() * CELL_GRID_SIZE);
            window.draw(cell_shape, global_transform);
        }

        window.display();
    }

    return 0;
}