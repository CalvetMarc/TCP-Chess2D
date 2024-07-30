#include "Window.h"

Window::Window(unsigned int width, unsigned int height, std::string title)
{
	window.create(sf::VideoMode(width, height), title);
	
}

void Window::AddButton(Button* bt)
{
	_mutex.lock();

	buttons.push_back(bt); //Afegir boto a la llista de botons per gestionar clicks
	objectsToDraw.push_back(bt); //Afegir boto a la llista de Drawable per renderitzarlo

	_mutex.unlock();
}

void Window::RunWindowLoop()
{
	while (window.isOpen())
	{
		window.clear(sf::Color::Black);

		for (sf::Drawable* drawable : objectsToDraw)
		{
			if (drawable != nullptr)
			{
				window.draw(*drawable); //Pintar els renderizables
			}
		}
		window.display();

		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::MouseButtonPressed:

				if (event.mouseButton.button == sf::Mouse::Left)
				{
					sf::Vector2i clickPixelPos = { event.mouseButton.x, event.mouseButton.y };
					sf::Vector2f worldPos = window.mapPixelToCoords(clickPixelPos);

					for (auto it = buttons.rbegin(); it != buttons.rend(); it++)
					{
						Button* bt = *it;
						if (bt->CheckBounds(worldPos.x, worldPos.y)) //Revisar si sha clickat algun boto
						{
							lastClickedDownButton = bt;
							break;
						}
					}
				}

				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					sf::Vector2i clickPixelPos = { event.mouseButton.x, event.mouseButton.y };
					sf::Vector2f worldPos = window.mapPixelToCoords(clickPixelPos);


					if (lastClickedDownButton != nullptr && lastClickedDownButton->CheckBounds(worldPos.x, worldPos.y)) //Si havia algun boto clickat el deixem de clickar
					{
						lastClickedDownButton->onClick();
						lastClickedDownButton = nullptr;
					}
					else
					{
						//
					}
					
				}
				break;
			case sf::Event::MouseWheelScrolled:
				//No hace falta
				break;
			case sf::Event::TextEntered:
				break;
			default:
				break;
			}
		}
	}
}
