#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "Pieces.h"
#include "vector"




class ChessBoard {
private:
    const float TOTALWIDTH = 1000;
    const float GAMEWIDTH = 650;
    const float HEIGTH = 650;
    std::vector<sf::RectangleShape>chatUI;
    sf::Text sfInputText, chatTitle;
    std::string inputText;
public:
    void loadtextures(sf::Texture texture[64]);
    void loadboard(sf::Texture texture[64], sf::RectangleShape rectangle[64], sf::Sprite sprite[64]);
    void run();

};
