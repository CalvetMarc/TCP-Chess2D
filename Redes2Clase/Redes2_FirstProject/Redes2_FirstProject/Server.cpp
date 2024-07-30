#include "Server.h"
#include <iostream>
#include <string>
#include <SFML/Network.hpp>
#include "Chat.h"
#include "SocketsManager.h"

Server* Server::instance = nullptr;

Server* Server::GetInstance()
{
    if (instance == nullptr)
        instance = new Server();

    return instance;
}

Server::Server()
{   
    //Al crear un SocketsManager hem de passar una funcio lambda al constructor
    //que es guardara a una variable privada de la clase
    //per tal que quan es conecti un TcpSocket pasarlo com a parametre. 
    //(veure SocketsManager::AddSocket(TcpSocket* socket) per entendreho)
    //Per tant, tots els sockets que conectem implementaran la funcionalitat seguent:
        
    ipAddress = new sf::IpAddress();
    serverState = new bool(false);

    sM = new SocketsManager([this](int playerID, TcpSocket* socket)
    {

            std::cout << std::endl << "Socket connected: " << socket->getRemoteAddress().toString() << std::endl;

            socket->Subscribe(Castle, [socket, this, playerID](Packet packet) {
                CastleData data(packet, false);
                Packet p;
                p << data;

                int playerRoomID = -1;
                clientsInRoomMutex.lock();
                std::map<int, int>::iterator it = clientsInRoom.find(playerID); //Busquem a quina room esta el player q ens envia el ready
                if (it != clientsInRoom.end())
                    playerRoomID = it->second;
                clientsInRoomMutex.unlock();

                if (playerRoomID != -1) {
                    allRoomsMutex.lock();
                    for (auto it = allRooms->rooms.begin(); it != allRooms->rooms.end(); it++) {
                        if ((*it)->id == playerRoomID) {

                            (*it)->playersInRoomMutex.lock();
                            for (std::map<int, std::pair<std::string, TcpSocket*>>::iterator it2 = (*it)->playersInRoom.begin(); it2 != (*it)->playersInRoom.end(); it2++) {
                                if (it2->first != playerID)
                                    it2->second.second->Send(GameplayPackagesIds::Castle, p);
                            }
                            (*it)->playersInRoomMutex.unlock();

                            (*it)->spectatorsInRoomMutex.lock();
                            for (std::map<int, std::pair<std::string, TcpSocket*>>::iterator it2 = (*it)->spectatorsInRoom.begin(); it2 != (*it)->spectatorsInRoom.end(); it2++) {
                                if (it2->first != playerID)
                                    it2->second.second->Send(GameplayPackagesIds::Castle, p);
                            }
                            (*it)->spectatorsInRoomMutex.unlock();
                            break;
                        }
                    }
                    allRoomsMutex.unlock();
                }

            });

            socket->Subscribe(Promotion, [socket, this, playerID](Packet packet) {
                PromotionData data(packet, false);
                Packet p;
                p << data;

                int playerRoomID = -1;
                clientsInRoomMutex.lock();
                std::map<int, int>::iterator it = clientsInRoom.find(playerID); //Busquem a quina room esta el player q ens envia el ready
                if (it != clientsInRoom.end())
                    playerRoomID = it->second;
                clientsInRoomMutex.unlock();

                if (playerRoomID != -1) {
                    allRoomsMutex.lock();
                    for (auto it = allRooms->rooms.begin(); it != allRooms->rooms.end(); it++) {
                        if ((*it)->id == playerRoomID) {

                            (*it)->playersInRoomMutex.lock();
                            for (std::map<int, std::pair<std::string, TcpSocket*>>::iterator it2 = (*it)->playersInRoom.begin(); it2 != (*it)->playersInRoom.end(); it2++) {
                                if (it2->first != playerID)
                                    it2->second.second->Send(GameplayPackagesIds::Promotion, p);
                            }
                            (*it)->playersInRoomMutex.unlock();

                            (*it)->spectatorsInRoomMutex.lock();
                            for (std::map<int, std::pair<std::string, TcpSocket*>>::iterator it2 = (*it)->spectatorsInRoom.begin(); it2 != (*it)->spectatorsInRoom.end(); it2++) {
                                if (it2->first != playerID)
                                    it2->second.second->Send(GameplayPackagesIds::Promotion, p);
                            }
                            (*it)->spectatorsInRoomMutex.unlock();
                            break;
                        }
                    }
                    allRoomsMutex.unlock();
                }

            });

            socket->Subscribe(EnPassant, [socket, this, playerID](Packet packet) {
                EnPassantData data(packet, false);
                Packet p;
                p << data;

                int playerRoomID = -1;
                clientsInRoomMutex.lock();
                std::map<int, int>::iterator it = clientsInRoom.find(playerID); //Busquem a quina room esta el player q ens envia el ready
                if (it != clientsInRoom.end())
                    playerRoomID = it->second;
                clientsInRoomMutex.unlock();

                if (playerRoomID != -1) {
                    allRoomsMutex.lock();
                    for (auto it = allRooms->rooms.begin(); it != allRooms->rooms.end(); it++) {
                        if ((*it)->id == playerRoomID) {

                            (*it)->playersInRoomMutex.lock();
                            for (std::map<int, std::pair<std::string, TcpSocket*>>::iterator it2 = (*it)->playersInRoom.begin(); it2 != (*it)->playersInRoom.end(); it2++) {
                                if (it2->first != playerID)
                                    it2->second.second->Send(GameplayPackagesIds::EnPassant, p);
                            }
                            (*it)->playersInRoomMutex.unlock();

                            (*it)->spectatorsInRoomMutex.lock();
                            for (std::map<int, std::pair<std::string, TcpSocket*>>::iterator it2 = (*it)->spectatorsInRoom.begin(); it2 != (*it)->spectatorsInRoom.end(); it2++) {
                                if (it2->first != playerID)
                                    it2->second.second->Send(GameplayPackagesIds::EnPassant, p);
                            }
                            (*it)->spectatorsInRoomMutex.unlock();
                            break;
                        }
                    }
                    allRoomsMutex.unlock();
                }

            });

            socket->Subscribe(SelectPiece, [socket, this, playerID](Packet packet) {
                //Vector2 pos(packet);
                Packet newPack;
                Vector2 pos(packet, false);
                newPack << pos;

                int playerRoomID = -1;
                clientsInRoomMutex.lock();
                std::map<int, int>::iterator it = clientsInRoom.find(playerID); //Busquem a quina room esta el player q ens envia el ready
                if (it != clientsInRoom.end())
                    playerRoomID = it->second;
                clientsInRoomMutex.unlock();

                if (playerRoomID != -1) {
                    allRoomsMutex.lock();
                    for (auto it = allRooms->rooms.begin(); it != allRooms->rooms.end(); it++) {
                        if ((*it)->id == playerRoomID) {
                            
                            (*it)->playersInRoomMutex.lock();
                            for (std::map<int, std::pair<std::string, TcpSocket*>>::iterator it2 = (*it)->playersInRoom.begin(); it2 != (*it)->playersInRoom.end(); it2++) {
                                if (it2->first != playerID)
                                    it2->second.second->Send(GameplayPackagesIds::SelectPiece, newPack);
                            }
                            (*it)->playersInRoomMutex.unlock();

                            (*it)->spectatorsInRoomMutex.lock();
                            for (std::map<int, std::pair<std::string, TcpSocket*>>::iterator it2 = (*it)->spectatorsInRoom.begin(); it2 != (*it)->spectatorsInRoom.end(); it2++) {
                                if (it2->first != playerID)
                                    it2->second.second->Send(GameplayPackagesIds::SelectPiece, newPack);
                            }
                            (*it)->spectatorsInRoomMutex.unlock();
                            break;
                        }
                    }
                    allRoomsMutex.unlock();
                }
            });

            socket->Subscribe(Deselect, [socket, this, playerID](Packet packet) {
                //Vector2 pos(packet);
                int playerRoomID = -1;
                clientsInRoomMutex.lock();
                std::map<int, int>::iterator it = clientsInRoom.find(playerID); //Busquem a quina room esta el player q ens envia el ready
                if (it != clientsInRoom.end())
                    playerRoomID = it->second;
                clientsInRoomMutex.unlock();

                if (playerRoomID != -1) {
                    allRoomsMutex.lock();
                    for (auto it = allRooms->rooms.begin(); it != allRooms->rooms.end(); it++) {
                        if ((*it)->id == playerRoomID) {

                            (*it)->playersInRoomMutex.lock();
                            for (std::map<int, std::pair<std::string, TcpSocket*>>::iterator it2 = (*it)->playersInRoom.begin(); it2 != (*it)->playersInRoom.end(); it2++) {
                                if (it2->first != playerID)
                                    it2->second.second->Send(GameplayPackagesIds::Deselect, packet);
                            }
                            (*it)->playersInRoomMutex.unlock();

                            (*it)->spectatorsInRoomMutex.lock();
                            for (std::map<int, std::pair<std::string, TcpSocket*>>::iterator it2 = (*it)->spectatorsInRoom.begin(); it2 != (*it)->spectatorsInRoom.end(); it2++) {
                                if (it2->first != playerID)
                                    it2->second.second->Send(GameplayPackagesIds::Deselect, packet);
                            }
                            (*it)->spectatorsInRoomMutex.unlock();
                            break;
                        }
                    }
                    allRoomsMutex.unlock();
                }
            });

            socket->Subscribe(MovePiece, [socket, this, playerID](Packet packet) {
                //Vector2 pos(packet);
                Packet newPack;
                Vector2 pos(packet, false);
                newPack << pos;

                int playerRoomID = -1;
                clientsInRoomMutex.lock();
                std::map<int, int>::iterator it = clientsInRoom.find(playerID); //Busquem a quina room esta el player q ens envia el ready
                if (it != clientsInRoom.end())
                    playerRoomID = it->second;
                clientsInRoomMutex.unlock();

                if (playerRoomID != -1) {
                    allRoomsMutex.lock();
                    for (auto it = allRooms->rooms.begin(); it != allRooms->rooms.end(); it++) {
                        if ((*it)->id == playerRoomID) {

                            (*it)->playersInRoomMutex.lock();
                            for (std::map<int, std::pair<std::string, TcpSocket*>>::iterator it2 = (*it)->playersInRoom.begin(); it2 != (*it)->playersInRoom.end(); it2++) {
                                if (it2->first != playerID)
                                    it2->second.second->Send(GameplayPackagesIds::MovePiece, newPack);
                            }
                            (*it)->playersInRoomMutex.unlock();

                            (*it)->spectatorsInRoomMutex.lock();
                            for (std::map<int, std::pair<std::string, TcpSocket*>>::iterator it2 = (*it)->spectatorsInRoom.begin(); it2 != (*it)->spectatorsInRoom.end(); it2++) {
                                if (it2->first != playerID)
                                    it2->second.second->Send(GameplayPackagesIds::MovePiece, newPack);
                            }
                            (*it)->spectatorsInRoomMutex.unlock();
                            break;
                        }
                    }
                    allRoomsMutex.unlock();
                }
            });

            socket->Subscribe(Message, [socket, this, playerID](Packet packet) {
                ChatMessage mssg(packet);
                Packet newP;
                ChatMessage* mssg2 = new ChatMessage;
                *mssg2 = mssg;
                newP << mssg;
                int playerRoomID = -1;
                clientsInRoomMutex.lock();
                std::map<int, int>::iterator it = clientsInRoom.find(playerID); //Busquem a quina room esta el player q ens envia el ready
                if (it != clientsInRoom.end())
                    playerRoomID = it->second;
                clientsInRoomMutex.unlock();

                if (playerRoomID != -1) {
                    allRoomsMutex.lock();
                    for (auto it = allRooms->rooms.begin(); it != allRooms->rooms.end(); it++) {
                        if ((*it)->id == playerRoomID) {
                            (*it)->AddMessage(mssg2);
                            (*it)->playersInRoomMutex.lock();
                            for (std::map<int, std::pair<std::string, TcpSocket*>>::iterator it2 = (*it)->playersInRoom.begin(); it2 != (*it)->playersInRoom.end(); it2++) {
                                if (it2->first != playerID)
                                    it2->second.second->Send(Message, newP);
                            }
                            (*it)->playersInRoomMutex.unlock();

                            (*it)->spectatorsInRoomMutex.lock();
                            for (std::map<int, std::pair<std::string, TcpSocket*>>::iterator it2 = (*it)->spectatorsInRoom.begin(); it2 != (*it)->spectatorsInRoom.end(); it2++) {
                                if (it2->first != playerID)
                                    it2->second.second->Send(GameplayPackagesIds::Message, newP);
                            }
                            (*it)->spectatorsInRoomMutex.unlock();
                            break;
                        }
                    }
                    allRoomsMutex.unlock();
                }
            });

            socket->Subscribe(MessagesRequest, [socket, this, playerID](Packet packet) {
                ChatMessage mssg(packet);
                int playerRoomID = -1;
                clientsInRoomMutex.lock();
                std::map<int, int>::iterator it = clientsInRoom.find(playerID); //Busquem a quina room esta el player q ens envia el ready
                if (it != clientsInRoom.end())
                    playerRoomID = it->second;
                clientsInRoomMutex.unlock();

                if (playerRoomID != -1) {
                    allRoomsMutex.lock();
                    for (auto it = allRooms->rooms.begin(); it != allRooms->rooms.end(); it++) {
                        if ((*it)->id == playerRoomID) {
                            Packet p;
                            (*it)->GetAllMessages(p);
                            socket->Send(MessagesResponse, p);
                            break;
                        }
                    }
                    allRoomsMutex.unlock();
                }
            });

            socket->Subscribe(OnLogin, [socket, this, playerID](Packet packet) {
                
                LoginRequestData data(packet);
                
                Packet roomsData;

                allRoomsMutex.lock();
                if (allRooms == nullptr)
                    allRooms = new RoomsUpdateData(roomsData, true); //Si encara no hi havia lobby, creem una nova amb una sala default
                else
                    allRooms->Code(roomsData); //Si el lobby ya existia guardem les salas que hi ha
                allRoomsMutex.unlock();   

                playersInLobbyMutex.lock();
                playersInLobby.insert(std::make_pair(playerID, std::make_pair(data.name, socket))); //Guardem el player i socket als clients q estan al lobby
                playersInLobbyMutex.unlock();

                if (socket->Send(MatchMakingPackagesIds::RoomsUpdate, roomsData)) {
                    std::cout << "Sever send roomupdate with a number of rooms of: " << std::endl;
                }
                else {
                    std::cout << "Sever failed send roomupdate" << std::endl;
                }
            });

            socket->Subscribe(CreateRoomRequest, [socket, this, playerID](Packet packet) {

                std::string newRoomName;
                packet >> newRoomName;

                roomsIdMutex.lock();
                sf::Uint64 newId = roomsId;
                roomsId++;
                roomsIdMutex.unlock();

                Packet allRoomsData;

                allRoomsMutex.lock();
                allRooms->rooms.push_back(new RoomData(newId, newRoomName));
                allRooms->Code(allRoomsData);
                allRoomsMutex.unlock();

                Packet responseToCreator;
                responseToCreator << newId;

                if (socket->Send(MatchMakingPackagesIds::CreateRoomResponse, responseToCreator)) { //enviem a qui ha creat la sala la id que se li ha assignat
                    std::cout << "Server send new room id to creator. ID: " << newId << std::endl;
                }
                else {
                    std::cout << "Server failed to send new room id to creator" << std::endl;
                }

                playersInLobbyMutex.lock();
                for (auto it = playersInLobby.begin(); it != playersInLobby.end(); it++){ //enviem a tots els altres de la lobby la nova sala que sha creat
                    if (it->first != playerID) {
                        if (it->second.second->Send(MatchMakingPackagesIds::RoomsUpdate, allRoomsData)) {
                            std::cout << "Server send new roomupdates to all lobby: " << std::endl;
                        }
                        else {
                            std::cout << "Error on server send new roomupdates to all lobby: " << std::endl;
                        }
                    }
                }
                playersInLobbyMutex.unlock();
            });

            socket->Subscribe(JoinRoomPlayerRequest, [socket, this, playerID] (Packet packet) {
                sf::Uint64 roomId;
                packet >> roomId;

                allRoomsMutex.lock();
                for (auto it = allRooms->rooms.begin(); it != allRooms->rooms.end(); it++) { 
                    if ((*it)->id == roomId) {
                        playersInLobbyMutex.lock();
                        std::map<int, std::pair< std::string, TcpSocket*>>::iterator pairIt = playersInLobby.find(playerID); //Si trobem el client de la lobby i hiha lloc a la room es pot unir, sino error
                        if (pairIt != playersInLobby.end()) {
                            std::pair<bool, std::pair<bool, CPVector<Address>>> addPlayerResults = (*it)->AddPlayerInRoom(*pairIt);
                            if (addPlayerResults.first) {
                                clientsInRoomMutex.lock();
                                clientsInRoom.insert(std::make_pair(playerID, roomId));
                                clientsInRoomMutex.unlock();

                                Packet p;
                                EnterAsPlayerResponseData data(addPlayerResults.second.first, addPlayerResults.second.second, p); //////////////////////////

                                if (pairIt->second.second->Send(OnEnterAsPlayer, p)) { //Enviem adresses i isFirst
                                    std::cout << "Server send to client that was able to enter room as player" << std::endl;
                                }
                                else {
                                    std::cout << "ERROR on server send to client that was able to enter room as player" << std::endl;
                                }
                                playersInLobby.erase(playerID); //Treiem el client del lobby perque ara esta a una sala 
                            }
                            else {
                                if (pairIt->second.second->Send(OnEnterAsPlayerFailure)) {
                                    std::cout << "Server send to client that was NOT able to enter room as player" << std::endl;
                                }
                                else {
                                    std::cout << "ERROR on server send to client that was NOT able to enter room as player" << std::endl;
                                }
                            }                            
                        }
                        
                        playersInLobbyMutex.unlock();
                        break;
                    }
                }
                allRoomsMutex.unlock();
            });

            socket->Subscribe(JoinRoomSpectatorRequest, [socket, this, playerID](Packet packet) {
                sf::Uint64 roomId;
                packet >> roomId;

                allRoomsMutex.lock();
                for (auto it = allRooms->rooms.begin(); it != allRooms->rooms.end(); it++) {
                    if ((*it)->id == roomId) {
                        playersInLobbyMutex.lock();
                        std::map<int, std::pair< std::string, TcpSocket*>>::iterator pairIt = playersInLobby.find(playerID); //Si trobem el client de la lobby i hiha lloc a la room es pot unir, sino error
                        if (pairIt != playersInLobby.end()) {   
                            clientsInRoomMutex.lock();
                            clientsInRoom.insert(std::make_pair(playerID, roomId));
                            clientsInRoomMutex.unlock();

                            Packet p;
                            EnterAsSpectatorResponseData data((*it)->AddSpectatorInRoom(*pairIt), p);
                            if (pairIt->second.second->Send(OnEnterAsSpectator, p)) { //Enviem addresses
                                std::cout << "Server send to client that was able to enter room as spectator" << std::endl;
                            }
                            else {
                                std::cout << "ERROR on server send to client that was able to enter room as spectator" << std::endl;
                            }
                            playersInLobby.erase(playerID); //Treiem el client del lobby perque ara esta a una sala                                                        
                        }
                        
                        playersInLobbyMutex.unlock();
                        break;
                    }
                }
                allRoomsMutex.unlock();
            });

            socket->Subscribe(SetReady, [socket, this, playerID](Packet packet) {
                int playerRoomID = -1;
                clientsInRoomMutex.lock();
                std::map<int, int>::iterator it = clientsInRoom.find(playerID); //Busquem a quina room esta el player q ens envia el ready
                if (it != clientsInRoom.end())
                    playerRoomID = it->second;
                clientsInRoomMutex.unlock();

                if (playerRoomID != -1) {
                    allRoomsMutex.lock();
                    CPVector<RoomData>::iterator roomIt;
                    for (auto it = allRooms->rooms.begin(); it != allRooms->rooms.end(); it++) {
                        if ((*it)->id == playerRoomID) {
                            (*it)->playersReady++; //Sumem que un player mes ya esta preparat
                            if ((*it)->playersReady >= 2) { //Si hi han 2 de preparats enviem a tothom que comença la partida ////////////////////////////////////////////////////////////////

                                (*it)->playersInRoomMutex.lock();
                                for (std::map<int, std::pair<std::string, TcpSocket*>>::iterator it2 = (*it)->playersInRoom.begin(); it2 != (*it)->playersInRoom.end(); it2++) {
                                    it2->second.second->Send(GameStart, Packet());
                                }
                                (*it)->playersInRoomMutex.unlock();

                                (*it)->spectatorsInRoomMutex.lock();
                                for (std::map<int, std::pair<std::string, TcpSocket*>>::iterator it2 = (*it)->spectatorsInRoom.begin(); it2 != (*it)->spectatorsInRoom.end(); it2++) {
                                    it2->second.second->Send(GameStart, Packet());
                                }
                                (*it)->spectatorsInRoomMutex.unlock();

                            }
                            roomIt = it;
                            break;
                        }
                    }
                    
                    allRoomsMutex.unlock();
                }
            });

            socket->SubscribeOnDisconnect([](TcpSocket* socket) {//El TcpSocket que passem com a parametre te una variable list de lambdas a executar quan es desconecti.
                //En aquest cas passem una lambda que fa un print del TcpSocket que sha desconectat

                std::cout << std::endl << "Socket disconected: " << socket->getRemoteAddress().toString() << std::endl;
            });
    });

}

void Server::Run(unsigned short _port, sf::RenderWindow* _appWindow, sf::Font* _font)
{  
    port = _port;
    appWindow = _appWindow;
    font = _font;

    HandleServerState();
}

void Server::HandleServerState()
{
    ServerStateManager serverStateManager(sM, appWindow, *font, ipAddress, serverState, port);
}



