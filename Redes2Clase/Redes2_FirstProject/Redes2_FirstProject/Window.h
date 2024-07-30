#pragma once
#include <list>
#include <mutex>

#include "Button.h"

class Window
{
public:
	/// <summary>
	/// El constructor crea la ventana y no se puede tocar desde otro thread que no sea el que la ha creado
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="title"></param>
	Window(unsigned int width = 800, unsigned int height = 600, std::string title = "Window");
	void AddButton(Button* bt);
	void RunWindowLoop();

private:

	std::mutex _mutex;
	sf::RenderWindow window;
	sf::Text text;
	Button* lastClickedDownButton;
	std::list<Button*> buttons;
	std::list<sf::Drawable*> objectsToDraw;


};

