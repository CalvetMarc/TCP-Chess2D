#include "AppManager.h"
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include "ConsoleControl.h"
#include "Client.h"
#include "Server.h"

AppManager* AppManager::instance = nullptr;

AppManager* AppManager::GetInstance()
{
    if (instance == nullptr)
        instance = new AppManager();
    return instance;
} 

void AppManager::Run()
{
    SelectMode();
}

AppManager::AppManager()
{
    font = new sf::Font();
    font->loadFromFile("Minecraft.ttf");
}

void AppManager::SelectMode()
{
    appWindow = new sf::RenderWindow(sf::VideoMode(), "");
    bool* modeIsServer = new bool;
    SelectAppModeManager(appWindow, *font, modeIsServer);

    if (*modeIsServer)
        Server::GetInstance()->Run(3001, appWindow, font);
    else
        Client::GetInstance()->Run(3001, appWindow, font);
}
