#include "Button.h"

Button::Button(float x, float y, std::string texturePath, sf::IntRect rect = sf::IntRect(0, 0, 0, 0))
{
	if (rect == sf::IntRect(0, 0, 0, 0)) {
		if (!texture.loadFromFile(texturePath))
		{
			//manage Error
		}
	}
	else {
		if (!texture.loadFromFile(texturePath, rect))
		{
			//manage Error
		}
	}
	this->setPosition(x, y);
	this->setTexture(texture);

}



bool Button::CheckBounds(unsigned int x, unsigned int y)
{
	return this->getGlobalBounds().contains(x,y);
}


