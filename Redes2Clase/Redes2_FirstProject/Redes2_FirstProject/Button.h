#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

class Button: public sf::Sprite
{
public:

	typedef std::function<void()> OnClick;
	OnClick onClick = [](){};

	Button(float x, float y, std::string texturePath, sf::IntRect rect); //Mas tarde habra que pasarle la textura ya cargada y no el path para no cargarla 20 veces

	bool CheckBounds(unsigned int x, unsigned int y);

private:

	sf::Texture texture;


};

