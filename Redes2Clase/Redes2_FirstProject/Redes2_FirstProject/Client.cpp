#include "Client.h"
#include <iostream>
#include <string>
#include <SFML/Network.hpp>
#include "Chat.h"
#include "SocketsManager.h"
#include "Structures.h"
#include "ChessManager.h"

Client* Client::instance = nullptr;

Client* Client::GetInstance()
{
    if (instance == nullptr)
        instance = new Client();

    return instance;
}

Client::Client()
{
    //Al crear un SocketsManager hem de passar una funcio lambda al constructor
    //que es guardara a una variable privada de la clase
    //per tal que quan es conecti un TcpSocket pasarlo com a parametre. 
    //(veure SocketsManager::AddSocket(TcpSocket* socket) per entendreho)
    //Per tant, tots els sockets que conectem implementaran la funcionalitat seguent:

    name = new std::string();    
    lobbyMutex = new std::mutex();
    currentState = new std::atomic<ClientState>(ClientState::DISCONNECTED);
    currentSeed = new std::atomic <sf::Uint64>(0);
    gameReady = new std::atomic <bool>(false);
    allMessages = new CPList<ChatMessage> ();

    int nada = 0;
    sM = new SocketsManager([this](int aux, TcpSocket* socket)
    {     

        std::cout << std::endl << "Socket connected: " << socket->getRemoteAddress().toString() << std::endl;   
        serverSocket = socket;

        //El TcpSocket que passem com a parametre te una variable map de keys y lambdas per tal que cada
        //instruccio que rebi el socket (key) corresponngui a una accio concreta (lambda).
        //En aquest cas, volem que la key "Message" tingui una lambda que printi el missatge que 
        //hi ha dins el paquet que hem rebut y envii una resposta a que ens ha enviat el missatge
                
        socket->Subscribe(MatchMakingPackagesIds::GameStart, [socket, this](Packet p) {
            *gameReady = true;
        });

        serverSocket->Subscribe(GameplayPackagesIds::Message, [this](Packet packet) {

            ChatMessage messageInfo = ChatMessage(packet);

            ChessManager::GetInstace()->MessageReceived(messageInfo);
        });

        serverSocket->Subscribe(GameplayPackagesIds::MessagesResponse, [this](Packet packet) {

            ChatMessagesResponse* cmr = new ChatMessagesResponse(packet);
            ChessManager::GetInstace()->LoadMessageHistory(cmr->messages);
        });

        socket->Subscribe(MatchMakingPackagesIds::RoomsUpdate, [socket, this](Packet p) { 

            std::cout << "Client Received a RoomUpdate" << std::endl;

            lobbyMutex->lock();
            //delete lobbyInfo;
            if (lobbyInfo == nullptr)
                lobbyInfo = new RoomsUpdateData(p, false); //Si encara no teniem info del lobby la creem
            else {
                lobbyInfo->rooms.clear(); //Si ya teniem info del lobby borrem les sales que teniem i guardem les noves que ens han arribat del servidor
                lobbyInfo->Decode(p);
            }

            lobbyMutex->unlock();

            if (*currentState == ClientState::DISCONNECTED) //Si encara no estavem al lobby actualitzem el estado
                *currentState = ClientState::INLOBBY;

            /**conexionDone = true;   */         
        });

        socket->Subscribe(MatchMakingPackagesIds::CreateRoomResponse, [socket, this](Packet p) {
            sf::Uint64 myNewRoom;
            p >> myNewRoom;
            
            Packet packetToSend;
            JoinRoomSpectatorRequestData dat(myNewRoom, packetToSend);

            if (socket->Send(JoinRoomSpectatorRequest, packetToSend)) { //Quan el server ens respon amb la id q ha asignat a la nova sala que em creat, demanem unirnos sempre com a spectator 
                std::cout << "Client send to server that wants to join his new room as Spectator" << std::endl;
            }
            else {
                std::cout << "ERROR on client sending to server that wants to join his new room as Spectator" << std::endl;
            }
        });

        socket->Subscribe(MatchMakingPackagesIds::OnEnterAsPlayer, [socket, this](Packet p) {    

            EnterAsPlayerResponseData data(p);
            isFirst = data.isFirst;
            clientsForP2P = data.otherP2PClients;
            *currentState = ClientState::PLAYER;
            std::cout << "Numero de adress: " + std::to_string(clientsForP2P.size()) << std::endl;

            for (int i = 0; i < clientsForP2P.size(); i++) {
                std::cout << "Adress: " + clientsForP2P[i]->ip << std::endl;
            }
        });

        socket->Subscribe(MatchMakingPackagesIds::OnEnterAsPlayerFailure, [socket, this](Packet p) {
            *currentSeed = 0;
        });

        socket->Subscribe(MatchMakingPackagesIds::OnEnterAsSpectator, [socket, this](Packet p) {            

            EnterAsSpectatorResponseData data(p);
            *currentState = ClientState::SPECTATOR;
            clientsForP2P = data.otherP2PClients;
        });        

        socket->Subscribe(SelectPiece, [socket, this](Packet packet) {
            Vector2 pos(packet, false);
            ChessManager::GetInstace()->PieceSelected(pos);
        });

        socket->Subscribe(Deselect, [socket, this](Packet packet) {
            ChessManager::GetInstace()->PieceDeselected();
        });

        socket->Subscribe(MovePiece, [socket, this](Packet packet) {
            Vector2 pos(packet, false);
            ChessManager::GetInstace()->PieceMove(pos);
        });

        socket->Subscribe(EnPassant, [socket, this](Packet packet) {
            EnPassantData data(packet, false);
            ChessManager::GetInstace()->EnPasssantMove(data);
        });

        socket->Subscribe(Castle, [socket, this](Packet packet) {
            CastleData data(packet, false);
            ChessManager::GetInstace()->CastleMove(data);
        });

        socket->Subscribe(Promotion, [socket, this](Packet packet) {
            PromotionData data(packet, false);
            ChessManager::GetInstace()->PromotionMove(data);
        });

        socket->SubscribeOnDisconnect([](TcpSocket* socket) {//El TcpSocket que passem com a parametre te una variable list de lambdas a executar quan es desconecti.
                                                            //En aquest cas passem una lambda que fa un print del TcpSocket que sha desconectat

            std::cout << std::endl << "Socket disconected: " << socket->getRemoteAddress().toString() << std::endl;
        });


        Packet loginPacket;
        LoginRequestData loginData(*name, loginPacket); //Al conectarnos al servidor li enviem el nom
        if (socket->Send(OnLogin, loginPacket))
            std::cout << "Done" << std::endl;
        else
            std::cout << "Merda" << std::endl;
    });    
    
}

void Client::Run(unsigned short _port, sf::RenderWindow* _appWindow, sf::Font* _font)
{
    appWindow = _appWindow;
    font = _font;

    StartLogIn();
    Lobby();    
    InGame();
}
void Client::StartLogIn()
{
    LogInManager logInManager(sM, name, appWindow, *font, currentState);    
}

void Client::Lobby()
{    

    LobbyManager lobbyManager(serverSocket, appWindow, *font, lobbyInfo, currentState, lobbyMutex, currentSeed);
}

void Client::InGame()
{   
    ChessManager::GetInstace()->RunGame(appWindow, currentState, currentSeed, isFirst, gameReady, sM, clientsForP2P, serverSocket, *font, *name);
}
