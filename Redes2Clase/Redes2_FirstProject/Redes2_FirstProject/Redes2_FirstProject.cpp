#pragma once
#include <iostream>
#include "AppManager.h"
#include <SFML/Graphics.hpp>
#include "ChessManager.h"


int main()
{
    /*sf::RenderWindow* appWindow = new sf::RenderWindow(sf::VideoMode(TOTALWIDTH, TOTALHEIGTH), "Chess Online!");
    ChessManager::GetInstace()->RunGame();

    return 0;*/

    AppManager::GetInstance()->Run();    
    
    return 0;
} 