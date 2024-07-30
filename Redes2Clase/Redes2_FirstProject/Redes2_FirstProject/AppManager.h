#pragma once
#include <iostream>
#include "Structures.h"

class AppManager
{
public:
	static AppManager* GetInstance();
	void Run();
private:
	char mode;
	sf::Font* font; 
	AppManager();
	static AppManager* instance;	
	sf::RenderWindow* appWindow;
	void SelectMode();
};

