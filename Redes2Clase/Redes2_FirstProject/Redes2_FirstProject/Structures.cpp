#include "Structures.h"
#include <iostream>
#include "SocketsManager.h"
#include "TcpSocket.h"

Vector2 Vector2::GetDirection(const Vector2& other) const
{
	Vector2 dir;
	if (other.row < this->row)
		dir.row = -1;
	else if (other.row == this->row)
		dir.row = 0;
	else
		dir.row = 1;

	if (other.column < this->column)
		dir.column = -1;
	else if (other.column == this->column)
		dir.column = 0;
	else
		dir.column = 1;
	return dir;
}


ServerStateManager::ServerStateManager(SocketsManager* sM, sf::RenderWindow* appWindow, sf::Font font, sf::IpAddress* ipAddress, bool* serverState, unsigned short port)
{
    appWindow->setSize(sf::Vector2u(SERVERUPWIDTH, SERVERUPHEIGHT));
    appWindow->setView(sf::View(sf::FloatRect(0, 0, SERVERUPWIDTH, SERVERUPHEIGHT)));
    appWindow->setTitle("LOG IN");

    sf::RectangleShape button;
    button.setFillColor(sf::Color(0, 100, 0));
    button.setPosition(SERVERUPWIDTH * 0.35f, SERVERUPHEIGHT * 0.25f);
    button.setSize(sf::Vector2f(SERVERUPWIDTH * 0.3f, SERVERUPWIDTH * 0.3f));

    sf::Text buttonText;
    buttonText.setFont(font);
    buttonText.setCharacterSize(16);
    buttonText.setPosition(SERVERUPWIDTH * 0.41f, SERVERUPHEIGHT * 0.38f);
    buttonText.setString("TURN\n   UP"); //" TURN \n DOWN"
    buttonText.setFillColor(sf::Color::White);

    sf::Text ipText;
    ipText.setFont(font);
    ipText.setCharacterSize(16);
    ipText.setPosition(SERVERUPWIDTH * 0.16f, SERVERUPHEIGHT * 0.8f);
    ipText.setString("");
    ipText.setFillColor(sf::Color::White);

    appWindow->clear();
    appWindow->draw(button);
    appWindow->draw(buttonText);
    appWindow->draw(ipText);
    appWindow->display();

    bool firstClickDone = false;
    sf::Clock clickClock;
    const sf::Time clickInterval = sf::milliseconds(500); //Pq no pilli doble click fiquem cooldown

    while (appWindow->isOpen()) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*appWindow);        

        sf::Event event;
        while (appWindow->pollEvent(event))
        {
            if (event.type == sf::Event::Closed) { //Si donem a la creu de la finestra aquesta es tanca
                TurnServerDown(sM);
                appWindow->close();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && button.getGlobalBounds().contains(sf::Vector2f(mousePos)) && (clickClock.getElapsedTime() >= clickInterval || !firstClickDone)) {
                if (!*serverState) {
                    if (TurnServerUp(sM, ipAddress, port)) {
                        *serverState = true;
                        button.setFillColor(sf::Color::Red);
                        buttonText.setString(" TURN \n DOWN");
                        ipText.setString("");
                        ipText.setString("Listening on Ip: " + ipAddress->toString());

                        appWindow->clear();
                        appWindow->draw(button);
                        appWindow->draw(buttonText);
                        appWindow->draw(ipText);
                        appWindow->display();
                        if(firstClickDone)
                            clickClock.restart();
                        firstClickDone = true;
                    }                    
                }
                else {
                    if (TurnServerDown(sM)) {
                        *serverState = false;
                        button.setFillColor(sf::Color(0, 100, 0));
                        buttonText.setString(" TURN\n   UP");
                        ipText.setString("");

                        appWindow->clear();
                        appWindow->draw(button);
                        appWindow->draw(buttonText);
                        appWindow->draw(ipText);
                        appWindow->display();
                        if (firstClickDone)
                            clickClock.restart();
                        clickClock.restart();
                    }
                }
            }
        }        
    }

    std::cout << "Server down" << std::endl;
}

bool ServerStateManager::TurnServerUp(SocketsManager* sM, sf::IpAddress* ipAddress, unsigned short port)
{
    if (sM->StartListener(port)) //Intentarem començar a escoltar amb el listener del servidor
    {
        *ipAddress = sf::IpAddress::getLocalAddress();
        std::cout << "Listening on Ip: " << ipAddress->toString() << std::endl;
        sM->StartLoop(); //Si ho aconseguim començarem el bucle d'execucio 
        return true;
    }    
    std::cout << "Error on turning server up" << std::endl;
    return false;
}

bool ServerStateManager::TurnServerDown(SocketsManager* sM)
{
    if (!sM->EndListener()) { //Intentarem tancar el listeenr 
        std::cout << "Error on close listener" << std::endl;
        return false;
    }
    std::cout << "Server down" << std::endl;
    return true;
}

LogInManager::LogInManager(SocketsManager* sM, std::string* name, sf::RenderWindow* appWindow, sf::Font font, std::atomic<ClientState>* clientState)
{
    appWindow->setSize(sf::Vector2u(LOGINWIDTH, LOGINHEIGHT));
    appWindow->setView(sf::View(sf::FloatRect(0, 0, LOGINWIDTH, LOGINHEIGHT)));
    appWindow->setTitle("LOG IN");

    std::string ip;
    unsigned short port = 0;  

    // Dibujar el cuerpo de la flecha (un rectángulo)
    sf::RectangleShape arrowBody(sf::Vector2f(20, 10));
    arrowBody.setFillColor(sf::Color::Blue);

    // Dibujar la cabeza de la flecha (un triángulo)
    sf::ConvexShape arrowHead(3); // Un triángulo tiene 3 vértices
    arrowHead.setPoint(0, sf::Vector2f(0, 0));
    arrowHead.setPoint(1, sf::Vector2f(20, 10));
    arrowHead.setPoint(2, sf::Vector2f(0, 20));
    arrowHead.setFillColor(sf::Color::Blue);

    sf::RectangleShape rect1(sf::Vector2f(LOGINWIDTH * 0.84f, 30));
    rect1.setPosition(LOGINWIDTH * 0.09f, LOGINHEIGHT * 0.25f - 5);
    rect1.setFillColor(sf::Color::Transparent); // Rellenado transparente
    rect1.setOutlineThickness(5); // Grosor del contorno
    rect1.setOutlineColor(sf::Color::Blue);

    sf::RectangleShape rect2(sf::Vector2f(LOGINWIDTH * 0.84f, 30));
    rect2.setPosition(LOGINWIDTH * 0.09f, LOGINHEIGHT * 0.5f - 5);
    rect2.setFillColor(sf::Color::Transparent); // Rellenado transparente
    rect2.setOutlineThickness(5); // Grosor del contorno
    rect2.setOutlineColor(sf::Color::Blue);

    sf::RectangleShape rect3(sf::Vector2f(LOGINWIDTH * 0.84f, 30));
    rect3.setPosition(LOGINWIDTH * 0.09f, LOGINHEIGHT * 0.75f - 5);
    rect3.setFillColor(sf::Color::Transparent); // Rellenado transparente
    rect3.setOutlineThickness(5); // Grosor del contorno
    rect3.setOutlineColor(sf::Color::Blue);

    std::string defaultNameText = "Username: ", defaultIpText = "Server IP: ", defaultPortText = "Server port: ";
    std::string inputs[3];
    sf::Text typeNameText, typeIpText, typePortText, title, loadingText;
    typeNameText.setFont(font);
    typeIpText.setFont(font);
    typePortText.setFont(font);
    title.setFont(font);
    loadingText.setFont(font);

    typeNameText.setCharacterSize(16);
    typeIpText.setCharacterSize(16);
    typePortText.setCharacterSize(16);
    title.setCharacterSize(20);
    loadingText.setCharacterSize(25);

    typeNameText.setPosition(LOGINWIDTH * 0.1f, LOGINHEIGHT * 0.25f);
    typePortText.setPosition(LOGINWIDTH * 0.1f, LOGINHEIGHT * 0.5f);
    typeIpText.setPosition(LOGINWIDTH * 0.1f, LOGINHEIGHT * 0.75f);
    title.setPosition(LOGINWIDTH * 0.37f, LOGINHEIGHT * 0.075f);
    loadingText.setPosition(LOGINWIDTH * 0.35f, LOGINHEIGHT * 0.5f);

    typeNameText.setString(defaultNameText);
    typePortText.setString(defaultPortText);
    typeIpText.setString(defaultIpText);
    title.setString("LOG IN INFO");
    loadingText.setString("LOADING...");

    typeNameText.setFillColor(sf::Color::White);
    typePortText.setFillColor(sf::Color::White);
    typeIpText.setFillColor(sf::Color::White);
    title.setFillColor(sf::Color::White);
    loadingText.setFillColor(sf::Color::Blue);

    int line = 0;
    bool waitingAccept = false;

    while (*clientState == ClientState::DISCONNECTED) { //Bucle fins q ens conectem amb login i rebem les salas del lobby
        sf::Event event;
        while (appWindow->pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed) {  //Flechas per canviar de campo
                if (event.key.code == sf::Keyboard::Up)
                {
                    line -= 1;

                    if (line < 0)
                        line = 2;
                    else
                        line = line % 3;
                }
                else if (event.key.code == sf::Keyboard::Down)
                {
                    line += 1;
                    line = line % 3;
                }
                else if (event.key.code == sf::Keyboard::Enter && !inputs[line].empty()) { //Si fem enter i el campo actual no esta buit
                    if (!inputs[0].empty() && !inputs[1].empty() && !inputs[2].empty()) { //Si estan tots plens fem servir la info per conectarnos
                        *name = inputs[0];
                        port = std::stoi(inputs[1]);
                        ip = inputs[2];

                        if (!sM->ConnectToServer(ip, port)) //Intentarem conectar el socket al servidor
                        {
                            std::cout << "Conexion failed" << std::endl;
                            port = 0;
                            ip.clear();
                        }
                        else {
                            sM->StartLoop(); //Si ens conectem començarem a escoltar paquets
                            std::cout << "Conexion Done" << std::endl;
                            waitingAccept = true;
                        }
                        
                    }                    
                }
            }
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128 && event.text.unicode != 8 && event.text.unicode != 13 && inputs[line].size() < 37) { // Caracteres ASCII sense "backspace" i "enter"
                    inputs[line] += static_cast<char>(event.text.unicode);
                }
                else if (event.text.unicode == 8 && !inputs[line].empty()) { // Tecla "backspace"
                    inputs[line].pop_back(); //Treu lultim input
                }

            }

        }

        float arrowPosY;

        if (line == 0)
            arrowPosY = typeNameText.getPosition().y;  //ajusta posicio de la felcha segons el campo al que esta
        else if(line == 1)
            arrowPosY = typePortText.getPosition().y;
        else
            arrowPosY = typeIpText.getPosition().y;

        arrowBody.setPosition(3, arrowPosY + 5);
        arrowHead.setPosition(15, arrowPosY+0.19f);

        typeNameText.setString(defaultNameText + inputs[0]); //Cada campo amb el seu propi text
        typePortText.setString(defaultPortText + inputs[1]);
        typeIpText.setString(defaultIpText + inputs[2]);

        appWindow->clear();

        if (!waitingAccept) { //Si encara no hem enviat es pinta el formulario
            appWindow->draw(typeNameText);
            appWindow->draw(rect1);
            appWindow->draw(rect2);
            appWindow->draw(rect3);
            appWindow->draw(typePortText);
            appWindow->draw(typeIpText);
            appWindow->draw(title);
            appWindow->draw(arrowBody);
            appWindow->draw(arrowHead);
        }        
        else {
            appWindow->draw(loadingText); //Si ya s'ha enviat i estem esperant resposta es pinta un loading
        }

        appWindow->display();
    }

}

SelectAppModeManager::SelectAppModeManager(sf::RenderWindow* appWindow, sf::Font font, bool* modeisServer)
{
    appWindow->setSize(sf::Vector2u(MODEWIDTH, MODEHEIGHT));
    appWindow->setView(sf::View(sf::FloatRect(0, 0, MODEWIDTH, MODEHEIGHT)));
    appWindow->setTitle("LOG IN");

    sf::RectangleShape buttonServer;
    buttonServer.setFillColor(sf::Color(0, 0, 200));
    buttonServer.setPosition(MODEWIDTH * 0.13f, MODEHEIGHT * 0.36f);
    buttonServer.setSize(sf::Vector2f(MODEWIDTH * 0.3f, MODEHEIGHT * 0.3f));

    sf::RectangleShape buttonClient;
    buttonClient.setFillColor(sf::Color(0, 100, 0));
    buttonClient.setPosition(MODEWIDTH * 0.57f, MODEHEIGHT * 0.36f);
    buttonClient.setSize(sf::Vector2f(MODEWIDTH * 0.3f, MODEHEIGHT * 0.3f));

    sf::Text buttonServerText;
    buttonServerText.setFont(font);
    buttonServerText.setCharacterSize(16);
    buttonServerText.setPosition(MODEWIDTH * 0.19f, MODEHEIGHT * 0.47f); //MODEWIDTH * 0.4f, MODEHEIGHT * 0.075f
    buttonServerText.setString("SERVER"); //" TURN \n DOWN"
    buttonServerText.setFillColor(sf::Color::White);

    sf::Text buttonClientText;
    buttonClientText.setFont(font);
    buttonClientText.setCharacterSize(16);
    buttonClientText.setPosition(MODEWIDTH * 0.65f, MODEHEIGHT * 0.47f); //MODEWIDTH * 0.4f, MODEHEIGHT * 0.075f
    buttonClientText.setString("CLIENT"); //" TURN \n DOWN"
    buttonClientText.setFillColor(sf::Color::White);

    sf::Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(20);
    titleText.setPosition(MODEWIDTH * 0.25f, MODEHEIGHT * 0.075f);
    titleText.setString("SELECT APP MODE");
    titleText.setFillColor(sf::Color::White);

    appWindow->clear();
    appWindow->draw(buttonServer);
    appWindow->draw(buttonClient);
    appWindow->draw(buttonServerText);
    appWindow->draw(buttonClientText);
    appWindow->draw(titleText);
    appWindow->display();

    bool selectionDone = false;

    while (!selectionDone) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*appWindow);

        sf::Event event;
        while (appWindow->pollEvent(event))
        {
            if (event.type == sf::Event::Closed) { //Si donem a la creu de la finestra aquesta es tanca
                appWindow->close();
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                if (buttonServer.getGlobalBounds().contains(sf::Vector2f(mousePos))) { //Detecta si triem modo server
                    *modeisServer = true;
                    selectionDone = true;
                }
                else if(buttonClient.getGlobalBounds().contains(sf::Vector2f(mousePos))) { //Detecta si triem modo client
                    *modeisServer = false;
                    selectionDone = true;
                }
            }
        }
    }

}

LobbyManager::LobbyManager(TcpSocket* socket, sf::RenderWindow* appWindow, sf::Font font, RoomsUpdateData* lobbyInfo, std::atomic<ClientState>* clientState, std::mutex* lobbyMutex, std::atomic <sf::Uint64>*seed)
{
    appWindow->setSize(sf::Vector2u(LOBBYWIDTH, LOBBYHEIGHT));
    appWindow->setView(sf::View(sf::FloatRect(0, 0, LOBBYWIDTH, LOBBYHEIGHT)));
    appWindow->setTitle("LOBBY");    

    std::string input;
    bool showExtraRoom = false;

    float viewTopValue = 0;

    std::map<sf::Int64, std::pair<sf::RectangleShape, sf::RectangleShape>> roomsButtons;
    std::pair<sf::RectangleShape, sf::RectangleShape> newRoom;
    sf::RectangleShape backGroundButtons(sf::Vector2f(180, 230));
    backGroundButtons.setFillColor(sf::Color(169, 169, 169));

    sf::Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(35);
    titleText.setString("LOBBY"); //" TURN \n DOWN"
    titleText.setFillColor(sf::Color::White);
    titleText.setOrigin(titleText.getLocalBounds().width / 2, titleText.getLocalBounds().height / 2);
    titleText.setPosition(LOBBYWIDTH / 2 - 5, 30);

    sf::Text buttonPlayerText;
    buttonPlayerText.setFont(font);
    buttonPlayerText.setCharacterSize(16);
    buttonPlayerText.setString("Enter as\n Player"); //" TURN \n DOWN"
    buttonPlayerText.setFillColor(sf::Color::White);

    sf::Text buttonViewerText;
    buttonViewerText.setFont(font);
    buttonViewerText.setCharacterSize(16);
    buttonViewerText.setString("Enter as\n Viewer"); //" TURN \n DOWN"
    buttonViewerText.setFillColor(sf::Color::White);

    sf::Text roomDateText;
    roomDateText.setFont(font);
    roomDateText.setCharacterSize(15);
    roomDateText.setFillColor(sf::Color::Black);

    sf::Text roomNameText;
    roomNameText.setFont(font);
    roomNameText.setCharacterSize(20);
    roomNameText.setFillColor(sf::Color::Black);

    sf::Uint64 idJoined;
    bool sended = false, sendedJoin = false;

    while (*clientState == ClientState::INLOBBY) {

        RePositionUi(appWindow, font, lobbyInfo, lobbyMutex, roomsButtons, newRoom, showExtraRoom); //Recoloca totes les rooms cada update, aixi podem anar printant les noves

        sf::Vector2i mousePos = sf::Mouse::getPosition(*appWindow);
        appWindow->setView(sf::View(sf::FloatRect(0, viewTopValue, LOBBYWIDTH, LOBBYHEIGHT)));

        sf::Event event;
        while (appWindow->pollEvent(event))
        {
            if (event.type == sf::Event::Closed) { //Si donem a la creu de la finestra aquesta es tanca
                appWindow->close();
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                //TODO: Revisar a quina box ha donat
                for (auto it = roomsButtons.begin(); it != roomsButtons.end(); it++) {
                    if (it->second.first.getGlobalBounds().contains(sf::Vector2f(mousePos + sf::Vector2i(0,viewTopValue)))) {
                        if (it->first == -1) {
                            if (!showExtraRoom) {     //Si li donem al - del + el convertim en una nova sala que omplir el nom
                                it->second = newRoom;
                                showExtraRoom = true;

                                break;
                            }                            
                        }
                        else {
                            std::cout << "Enter as Player" << std::endl; 

                            Packet packetToSend;
                            JoinRoomPlayerRequestData dat(it->first, packetToSend);

                            if (socket->Send(JoinRoomPlayerRequest, packetToSend)) { //Si donem a una sala q volem entrar com a player ho enviem al server
                                std::cout << "Client send to server that wants to join his new room as Player" << std::endl;
                                sendedJoin = true;
                                idJoined = it->first;
                            }
                            else {
                                std::cout << "ERROR on client sending to server that wants to join his new room as Player" << std::endl;
                            }
                            
                            break;
                        }
                    }
                    else if (it->second.second.getGlobalBounds().contains(sf::Vector2f(mousePos + sf::Vector2i(0, viewTopValue)))) {
                        if (it->first == -1) {
                            if (!showExtraRoom) { //Si li donem al | del + el convertim en una nova sala que omplir el nom
                                it->second = newRoom;
                                showExtraRoom = true;

                                break;
                            }                            
                        }
                        else {
                            std::cout << "Enter as Viewer" << std::endl;

                            Packet packetToSend;
                            JoinRoomSpectatorRequestData dat(it->first, packetToSend);

                            if (socket->Send(JoinRoomSpectatorRequest, packetToSend)) { //Si donem a una sala q volem entrar com a spectator ho enviem al server
                                std::cout << "Client send to server that wants to join a room as Spectator" << std::endl;                                
                            }
                            else {
                                std::cout << "ERROR on client sending to server that wants to join a room as Spectator" << std::endl;
                            }

                            break;
                        }
                    }
                }
            }
            if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                if (event.mouseWheelScroll.delta > 0)
                {
                    viewTopValue -= 15;
                    if (viewTopValue < 0)
                        viewTopValue = 0;
                }
                else if (event.mouseWheelScroll.delta < 0)  //Per veure les sales de mes abaix amb la rodeta del mosue
                {
                    viewTopValue += 15;
                    float maxValue;

                    int filas = (roomsButtons.size() / 3);
                    if (filas < 2) {
                        maxValue = 0;
                    }
                    else {
                        maxValue = (filas - 1) * 250;
                    }

                    if (viewTopValue > maxValue)
                        viewTopValue = maxValue;
                }
            }
            if (event.type == sf::Event::KeyPressed) {                  
                
                if (event.key.code == sf::Keyboard::Enter && !input.empty() && !sended) { //Enter
                    //TODO: enviar la nova sala
                    Packet roomNamePck;
                    roomNamePck << input;
                    if (socket->Send(MatchMakingPackagesIds::CreateRoomRequest, roomNamePck)) { //Si estavem escribint el nom i donem al enter senvia el noim de la nova sala per crearla
                        sended = true;
                        std::cout << "client send new room name to server" << std::endl;
                    }
                    else {
                        std::cout << "error on send new room name to server" << std::endl;
                    }
                }
            }
            if (event.type == sf::Event::TextEntered && showExtraRoom) {
                if (event.text.unicode < 128 && event.text.unicode != 8 && event.text.unicode != 13 && input.size() < 14) { // Caracteres ASCII sense "backspace" i "enter" per escriure nom de la sala
                    input += static_cast<char>(event.text.unicode);
                }
                else if (event.text.unicode == 8 && !input.empty()) { // Tecla "backspace"
                    input.pop_back();
                }
            }
        }

        appWindow->clear();

        for (auto it = roomsButtons.begin(); it != roomsButtons.end(); it++) {
            
            backGroundButtons.setPosition(sf::Vector2f(it->second.first.getPosition().x - 16, it->second.first.getPosition().y - 30)); //Background gris de las salas
            if (it->first != -1 || showExtraRoom)  //El dibuixem sempre menys al simbol +
                appWindow->draw(backGroundButtons);
            appWindow->draw(it->second.first);
            appWindow->draw(it->second.second);   
            if (it->first == -1 && !showExtraRoom) {
                continue;
            }
            bool found = false;
            lobbyMutex->lock();
            for (auto it2 = lobbyInfo->rooms.begin(); it2 != lobbyInfo->rooms.end(); it2++) { //Fiquem el nom i data de cada sala
                if (sendedJoin && idJoined == (*it2)->id)
                    *seed = (*it2)->timeStamp;
                if (it->first == (*it2)->id) {
                    roomNameText.setString((*it2)->name);                    
                    roomDateText.setString(GetDate((*it2)->timeStamp));
                    found = true;
                    break;
                }

            }       
            lobbyMutex->unlock();
            if (!found) { //Si la sala no la trobem es perque es la que estem creant, no te data i el nom es el input
                roomNameText.setString(input);
                roomDateText.setString("-");
            }
            roomNameText.setOrigin(roomNameText.getLocalBounds().width / 2, roomNameText.getLocalBounds().height / 2);
            roomNameText.setPosition(it->second.first.getPosition().x + 75, it->second.first.getPosition().y - 21);   

            roomDateText.setOrigin(roomDateText.getLocalBounds().width / 2, roomDateText.getLocalBounds().height / 2);
            roomDateText.setPosition(it->second.second.getPosition().x + 75, it->second.second.getPosition().y + 97);

            buttonPlayerText.setPosition(it->second.first.getPosition().x + 40, it->second.first.getPosition().y + 25);
            buttonViewerText.setPosition(it->second.second.getPosition().x + 40, it->second.second.getPosition().y + 25);

            appWindow->draw(roomNameText);
            appWindow->draw(roomDateText);
            appWindow->draw(buttonPlayerText);
            appWindow->draw(buttonViewerText);
        }       
        appWindow->draw(titleText);

        appWindow->display();

    }
}

void LobbyManager::RePositionUi(sf::RenderWindow* appWindow, sf::Font font, RoomsUpdateData* lobbyInfo, std::mutex* lobbyMutex,
    std::map<sf::Int64, std::pair<sf::RectangleShape, sf::RectangleShape>>& roomsButtons, std::pair<sf::RectangleShape, sf::RectangleShape>& newRoom, bool showExtraRoom)
{
    roomsButtons.clear();
    lobbyMutex->lock();
    for (int i = 0; i <= lobbyInfo->rooms.size(); i++) {
        float boxX = 70 + 250 * (i % 3); //Columnas de 0 a 2
        float boxY = 175 + 250 * (i / 3); //Cada 3 es canvia de fila

        if (i != lobbyInfo->rooms.size()) {
            sf::RectangleShape buttonPlayer(sf::Vector2f(150, 90));
            buttonPlayer.setPosition(sf::Vector2f(boxX, boxY - 45));
            buttonPlayer.setFillColor(sf::Color(200, 0, 0));
            sf::RectangleShape buttonViewer(sf::Vector2f(150, 90));
            buttonViewer.setPosition(sf::Vector2f(boxX, boxY + 45));
            buttonViewer.setFillColor(sf::Color(0, 0, 180));


            roomsButtons.insert(std::make_pair(lobbyInfo->rooms[i]->id, std::make_pair(buttonPlayer, buttonViewer))); //Guardem la id de la sala i els seus botons
        }
        else {

            sf::RectangleShape buttonPlayerNewRoom(sf::Vector2f(150, 90)); 
            buttonPlayerNewRoom.setPosition(sf::Vector2f(boxX, boxY - 45));
            buttonPlayerNewRoom.setFillColor(sf::Color(200, 0, 0));
            sf::RectangleShape buttonViewerNewRoom(sf::Vector2f(150, 90));
            buttonViewerNewRoom.setPosition(sf::Vector2f(boxX, boxY + 45));
            buttonViewerNewRoom.setFillColor(sf::Color(0, 0, 180));

            newRoom.first = buttonPlayerNewRoom;
            newRoom.second = buttonViewerNewRoom;

            sf::RectangleShape buttonAddRoomHor(sf::Vector2f(70, 20));
            buttonAddRoomHor.setOrigin(buttonAddRoomHor.getLocalBounds().width / 2, buttonAddRoomHor.getLocalBounds().height / 2);
            buttonAddRoomHor.setPosition(sf::Vector2f(boxX + 75, boxY + 45));
            buttonAddRoomHor.setFillColor(sf::Color::White);

            sf::RectangleShape buttonAddRoomVer(sf::Vector2f(20, 70));
            buttonAddRoomVer.setOrigin(buttonAddRoomVer.getLocalBounds().width / 2, buttonAddRoomVer.getLocalBounds().height / 2);
            buttonAddRoomVer.setPosition(sf::Vector2f(boxX + 75, boxY + 45));
            buttonAddRoomVer.setFillColor(sf::Color::White);

            if(!showExtraRoom) //Al final fiquem sempre el simbol + o la sala que estem creant
                roomsButtons.insert(std::make_pair(-1, std::make_pair(buttonAddRoomHor, buttonAddRoomVer)));
            else
                roomsButtons.insert(std::make_pair(-1, newRoom));
        }
    }
    lobbyMutex->unlock();
}

std::string LobbyManager::GetDate(sf::Uint64 numDate)
{ 
    std::string date = std::to_string(numDate); //transforma el valor de timestamp a una data normal
    std::string year = date.substr(0, 4);
    std::string month = date.substr(4, 2);
    std::string day = date.substr(6, 2);
    std::string hour = date.substr(8, 2);
    std::string min = date.substr(10, 2);
    std::string sec = date.substr(12, 2);
    std::string dateFixed = day + '/' + month + '/' + year + " - " + hour + ':' + min + ':' + sec;

    return dateFixed;
}

ChatManager::ChatManager(sf::RenderWindow* _gameWindow, SocketsManager* _sM, std::string _name, sf::Font _font, TcpSocket* _serverSocket) : gameWindow(_gameWindow), sM(_sM), name(_name), font(_font), serverSocket(_serverSocket)
{
    sfInputText.setFont(font); // Establecer la fuente
    sfInputText.setCharacterSize(16); // Establecer el tamaño del texto
    sfInputText.setFillColor(sf::Color::White); // Establecer el color del texto
    sfInputText.setString("Enter text...");
    sfInputText.setOrigin(0, 0.5);
    sfInputText.setPosition(GAMEWIDTH + 15, TOTALHEIGTH - 35); //GAMEWIDTH + 15, TOTALHEIGTH - 35
    
    backGround = new sf::RectangleShape(sf::Vector2f(TOTALWIDTH - GAMEWIDTH, TOTALHEIGTH - 150));
    backGround->setOrigin(backGround->getGlobalBounds().width * 0.5f, backGround->getGlobalBounds().height * 0.5f);
    backGround->setPosition(GAMEWIDTH + ((TOTALWIDTH - GAMEWIDTH)*0.5f), TOTALHEIGTH * 0.535f);
    backGround->setFillColor(sf::Color(40, 40, 60)); //200, 80, 0

    backGroundTitle = new sf::RectangleShape(sf::Vector2f(TOTALWIDTH - GAMEWIDTH, backGround->getGlobalBounds().top));
    backGroundTitle->setOrigin(backGroundTitle->getGlobalBounds().width * 0.5f, backGroundTitle->getGlobalBounds().height * 0.5f);
    backGroundTitle->setPosition(GAMEWIDTH + ((TOTALWIDTH - GAMEWIDTH) * 0.5f), backGround->getGlobalBounds().top * 0.5f);
    backGroundTitle->setFillColor(sf::Color(20, 30, 50)); //200, 200, 130

    backGroundInput = new sf::RectangleShape(sf::Vector2f(TOTALWIDTH - GAMEWIDTH, 2*(TOTALHEIGTH - backGround->getGlobalBounds().height - backGroundTitle->getGlobalBounds().height)));
    backGroundInput->setOrigin(backGroundInput->getGlobalBounds().width * 0.5f, backGroundInput->getGlobalBounds().height * 0.5f);
    backGroundInput->setPosition(GAMEWIDTH + ((TOTALWIDTH - GAMEWIDTH) * 0.501f), sfInputText.getPosition().y + 35);
    backGroundInput->setFillColor(sf::Color(80, 120, 130)); //200, 200, 130

    chatTitle.setFont(font);
    chatTitle.setCharacterSize(32);
    chatTitle.setFillColor(sf::Color::White); //  
    chatTitle.setString("Room Chat");
    sf::FloatRect textRect = chatTitle.getLocalBounds();
    chatTitle.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    chatTitle.setPosition(GAMEWIDTH + (TOTALWIDTH - GAMEWIDTH) / 2, 50);
}

void ChatManager::ProcessMssgs()
{   
    std::string text = input.empty() ? "Enter text..." : input;
    sfInputText.setString(text);

    gameWindow->draw(*backGround);
    gameWindow->draw(*backGroundTitle);
    gameWindow->draw(*backGroundInput);
    gameWindow->draw(sfInputText);
    gameWindow->draw(chatTitle);

    processedMssgsMutex.lock();
    std::multimap<sf::Uint64, std::string> mssgsToPrint;
    for (std::multimap< sf::Uint64, std::string>::iterator it = processedMssgs.begin(); it != processedMssgs.end(); it++) {
        mssgsToPrint.insert(std::make_pair(GetTime() - it->first, it->second));
    }
    processedMssgsMutex.unlock();

    sf::Text mssg;
    mssg.setFont(font); // Establecer la fuente
    mssg.setCharacterSize(16); // Establecer el tamaño del texto
    mssg.setFillColor(sf::Color::White); // Establecer el color del texto
    mssg.setOrigin(0, 0.5);

    int iteration = 0;
    for (std::multimap< sf::Uint64, std::string>::iterator it = mssgsToPrint.begin(); it != mssgsToPrint.end(); it++) {
        iteration++;
        mssg.setString(it->second);
        mssg.setPosition(GAMEWIDTH + 15, (TOTALHEIGTH - 35) - (45 * iteration)); //GAMEWIDTH + 15, TOTALHEIGTH - 35
        if (mssg.getPosition().y < 100) {
            break;
        }
        else {
            gameWindow->draw(mssg);
        }
    }

}

void ChatManager::HandleInput(sf::Event event)
{
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && !input.empty()) {
        
        ChatMessage mssg(Address(name, sf::IpAddress::getLocalAddress().toString(), 3001), input, GetTime());
        input.clear();        
        LoadMessage(mssg);

        Packet p;
        p << mssg;
        serverSocket->Send(Message, p);
    }
    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode < 128 && event.text.unicode != 8 && event.text.unicode != 13 && sfInputText.getGlobalBounds().width < ((TOTALWIDTH - GAMEWIDTH)-20)) { // Caracteres ASCII sense "backspace" i "enter" per escriure nom de la sala
            input += static_cast<char>(event.text.unicode);
        }
        else if (event.text.unicode == 8 && !input.empty()) { // Tecla "backspace"
            input.pop_back();
        }
    }
}

void ChatManager::HandleInput()
{
    sf::Event event;
    while (gameWindow->pollEvent(event)) {
        HandleInput(event);
    }    
}

void ChatManager::LoadMessage(ChatMessage mssg)
{
    processedMssgsMutex.lock();
    processedMssgs.insert(std::make_pair(mssg.timeStamp, GetMssgTime(mssg.timeStamp) + mssg.user.name + ": " + mssg.message));
    processedMssgsMutex.unlock();
}

void ChatManager::LoadMessageList(CPList<ChatMessage> messages)
{
    processedMssgsMutex.lock();
    for (CPList<ChatMessage>::iterator it = messages.begin(); it != messages.end(); it++) {
        processedMssgs.insert(std::make_pair((*it)->timeStamp, GetMssgTime((*it)->timeStamp) + (*it)->user.name + ": " + (*it)->message));
    }
    processedMssgsMutex.unlock();
}

std::string ChatManager::GetMssgTime(sf::Uint64 timeStamp)
{   
    std::string date = std::to_string(timeStamp); //transforma el valor de timestamp a una data normal
    //std::string year = date.substr(0, 4);
    //std::string month = date.substr(4, 2);
    //std::string day = date.substr(4, 2);
    int zeros = 6 - date.size();
    std::string extra;
    for (int i = 0; i < zeros; i++) {
        extra += "0";
    }
    extra += date;
    std::string hour = extra.substr(0, 2);
    std::string min = extra.substr(2, 2);
    std::string sec = extra.substr(4, 2);
    std::string dateFixed = hour + ':' + min + "| ";

    return dateFixed;
}

sf::Uint64 ChatManager::GetTime()
{
    // Obtener la fecha y hora actual
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);
    // Construir la fecha y hora como una cadena de caracteres
    char datetime_str[20];
    std::strftime(datetime_str, sizeof(datetime_str), "%Y%m%d%H%M%S", local_time);
    // Convertir la cadena de caracteres a un sf::Uint64
    sf::Uint64 timestamp;
    std::sscanf(datetime_str, "%llu", &timestamp);
    timestamp %= 1000000;
    return timestamp % 1000000;
}
