#include "flashmessage.hpp"
#include "constants.hpp"

FlashMessage::FlashMessage()
    : milliseconds_to_display_(MILLISECONDS_DISPLAY_FLASH_MESSAGE)
    , milliseconds_to_fade_(MILLISECONDS_FADE_FLASH_MESSAGE)
    , is_displayed_(false)
    , base_color_(sf::Text::getFillColor())
{
}

FlashMessage::FlashMessage(const sf::Font& font, unsigned int character_size)
    : sf::Text("", font, character_size)
    , milliseconds_to_display_(MILLISECONDS_DISPLAY_FLASH_MESSAGE)
    , milliseconds_to_fade_(MILLISECONDS_FADE_FLASH_MESSAGE)
    , is_displayed_(false)
    , base_color_(sf::Text::getFillColor())
{
}

void FlashMessage::setFillColor(const sf::Color& color) {
    base_color_ = color;
    sf::Text::setFillColor(color);
}

const sf::Color& FlashMessage::getFillColor() const {
    return base_color_;
}

void FlashMessage::Display(const std::string& message) {
    is_displayed_ = true;
    setString(message);
    sf::Text::setFillColor(base_color_);
    timer_.restart();
}

void FlashMessage::Update() {
    sf::Int32 time_message_displayed = timer_.getElapsedTime().asMilliseconds();

    if(time_message_displayed < milliseconds_to_display_) {
        // No-op
    } else if(time_message_displayed < milliseconds_to_display_ + milliseconds_to_fade_) {
        sf::Color fade_color = sf::Color(base_color_);
        fade_color.a = static_cast<sf::Uint8>(255 - static_cast<float>(time_message_displayed - milliseconds_to_display_) / milliseconds_to_fade_ * 255);
        sf::Text::setFillColor(fade_color);
    } else {
        is_displayed_ = false;
    }
}

void FlashMessage::SetDisplayTime(sf::Int32 milliseconds_to_display) {
    milliseconds_to_display_ = milliseconds_to_display;
}

void FlashMessage::SetFadeTime(sf::Int32 milliseconds_to_fade) {
    milliseconds_to_fade_ = milliseconds_to_fade;
}

bool FlashMessage::is_displayed() const {
    return is_displayed_;
}
