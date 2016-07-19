#ifndef GAME_OF_LIFE_FLASH_MESSAGE_HPP
#define GAME_OF_LIFE_FLASH_MESSAGE_HPP

#include <SFML/Graphics.hpp>

class FlashMessage : public sf::Text {
public:
    FlashMessage();
    FlashMessage(const sf::Font& font, unsigned int character_size=30);

    void setColor(const sf::Color& color);
    const sf::Color& getColor() const;

    void Display(const std::string& message);
    void Update();

    void SetDisplayTime(sf::Int32 milliseconds_to_display);
    void SetFadeTime(sf::Int32 milliseconds_to_fade);

    bool is_displayed() const;

private:
    sf::Int32 milliseconds_to_display_;
    sf::Int32 milliseconds_to_fade_;
    bool is_displayed_;
    sf::Color base_color_;
    sf::Clock timer_;
};

#endif // GAME_OF_LIFE_FLASH_MESSAGE_HPP
