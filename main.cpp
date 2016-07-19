#include <SFML/Graphics.hpp>
#include <iostream>
#include "gameoflife.hpp"
#include "resources.hpp"
#include "flashmessage.hpp"
#include "constants.hpp"

int main(int argc, char* argv[]) {
    initialize_rules();
    CellContainer state;

    load_state("input.txt", state);
    CellContainer changed_cells(state);

    sf::RenderWindow window(sf::VideoMode(VIDEO_MODE_WIDTH, VIDEO_MODE_HEIGHT), "Conway's Game of Life");

    sf::RenderTexture render_texture;

    if(!render_texture.create(window.getSize().x, window.getSize().y)) {
        std::cerr << "Failed to create render texture\n";
        return -1;
    }

    sf::Shader shader;

    if(load_fragment_shader(shader, "crt.frag")) {
        shader.setParameter("texture", sf::Shader::CurrentTexture);
        shader.setParameter("textureSize", window.getSize().x, window.getSize().y);

        shader.setParameter("devicePixelRatio", DEVICE_PIXEL_RATIO);
        shader.setParameter("virtualResolution", VIRTUAL_RESOLUTION_WIDTH, VIRTUAL_RESOLUTION_HEIGHT);

        shader.setParameter("enableScanlines", ENABLE_SCANLINES);
        shader.setParameter("enablePixelRasterization", ENABLE_PIXEL_RASTERIZATION);

        shader.setParameter("enableScreenCurvature", ENABLE_SCREEN_CURVATURE);
        shader.setParameter("screenCurvature", SCREEN_CURVATURE);
    }

    sf::Clock generation_timer;

    sf::CircleShape cell_shape(CELL_SHAPE_SIZE / 2); // Takes a radius
    cell_shape.setFillColor(CELL_COLOR);

    sf::Font flash_message_font = load_font("Inconsolata-Regular.ttf");
    FlashMessage flash_message(flash_message_font, FLASH_MESSAGE_CHARACTER_SIZE);
    flash_message.setPosition(
		static_cast<float>(FLASH_MESSAGE_OFFSET_X),
		static_cast<float>(FLASH_MESSAGE_OFFSET_Y));
    flash_message.setColor(FLASH_MESSAGE_COLOR);

    // Frame to avoid artifacts on edges of texture when curvature is applied
    sf::RectangleShape frame_shapes[] {
        sf::RectangleShape(sf::Vector2f(1, VIDEO_MODE_HEIGHT)),
        sf::RectangleShape(sf::Vector2f(VIDEO_MODE_WIDTH, 1)),
    };
    for(auto& frame_shape: frame_shapes) {
        frame_shape.setFillColor(sf::Color::Black);
    }

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

                    if(is_auto_fit_enabled) {
                        flash_message.Display("Auto-fit disabled");
                        is_auto_fit_enabled = false;
                    }
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
                advance(state, changed_cells);
                generation_timer.restart();
            }
        }

        render_texture.clear(sf::Color::Black);

        for(const Cell& cell: state) {
            cell_shape.setPosition(
                static_cast<float>(cell.X() * CELL_GRID_SIZE),
				static_cast<float>(cell.Y() * CELL_GRID_SIZE));
            render_texture.draw(cell_shape, global_transform * viewport_transform);
        }

        if(flash_message.is_displayed()) {
            flash_message.Update();
            render_texture.draw(flash_message);
        }

        frame_shapes[0].setPosition(0, 0);
        render_texture.draw(frame_shapes[0]);

        frame_shapes[0].setPosition(VIDEO_MODE_WIDTH - 1, 0);
        render_texture.draw(frame_shapes[0]);

        frame_shapes[1].setPosition(0, 0);
        render_texture.draw(frame_shapes[1]);

        frame_shapes[1].setPosition(0, VIDEO_MODE_HEIGHT - 1);
        render_texture.draw(frame_shapes[1]);

        render_texture.display();

        const sf::Texture& texture = render_texture.getTexture();
        sf::Sprite sprite(texture);
        window.draw(sprite, &shader);
        window.display();
    }

    return 0;
}
