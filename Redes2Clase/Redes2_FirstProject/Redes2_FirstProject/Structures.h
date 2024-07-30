#pragma once
#pragma warning(disable : 4996)
#include <SFML/Graphics.hpp>
#include "Codable.h"
#include <thread>
#include <set>
#include <chrono>
#include <mutex>
#include "TcpSocket.h"

//INGAME
const float TOTALWIDTH = 1000;
const float GAMEWIDTH = 650;
const float TOTALHEIGTH = 700;
const float GAMEHEIGTH = 650;
const int ChessSideLength = 8;
//CLIENT LOGIN
const float LOGINWIDTH = 500;
const float LOGINHEIGHT = 450;
//SERVERUP
const float SERVERUPWIDTH = 300;
const float SERVERUPHEIGHT = 200;
//SELECTAPPMODE
const float MODEWIDTH = 400;
const float MODEHEIGHT = 350;
//LOBBY
const float LOBBYWIDTH = 800;
const float LOBBYHEIGHT = 600;

class SocketsManager;
class TcpSocket;

enum MatchMakingPackagesIds : Packet::PacketKey { 
	  OnLogin = 0 , CreateRoomRequest = 1, CreateRoomResponse = 2
	, RoomsUpdate = 3, JoinRoomPlayerRequest = 4, OnEnterAsPlayer = 5, OnEnterAsPlayerFailure = 6
	, JoinRoomSpectatorRequest = 7, OnEnterAsSpectator = 8, OnEnterAsSpectatorFailure = 9, ExitRoom = 10
	, SetReady = 11, GameStart = 12, 
};

enum GameplayPackagesIds : Packet::PacketKey {
	SelectPiece = 13, Deselect = 14, MovePiece = 15, Promotion = 16, Castle = 17, EnPassant = 18, Message = 19, MessagesRequest = 20, MessagesResponse = 21
};

enum GAMESTATE {
	PLAYING, WHITEWINS, BLACKWINS, DRAW
};

enum class ClientState { DISCONNECTED, INLOBBY, PLAYER, SPECTATOR };

enum PieceType {
	PawnType = 0,
	KnightType = 1,
	BishopType = 2,
	RookType = 3,
	QueenType = 4,
	KingType = 5
};

enum SpecialMove {
	NONE,
	ENPASSANT,
	CASTLING,
	PROMOTION
};

struct Vector2 : public ICodable {
	sf::Int32 column, row;
	inline Vector2() = default;
	inline Vector2(Packet& p, bool code) { if (code) Code(p); else Decode(p); };
	inline Vector2(int _row, int _column) : row(_row), column(_column) {}
	Vector2 GetDirection(const Vector2& other)const;
	inline Vector2 operator+(const Vector2& other) const { return Vector2(row + other.row, column + other.column); }
	inline Vector2 operator-(const Vector2& other) const { return Vector2(row - other.row, column - other.column); }
	inline Vector2 operator*(const Vector2& other) const { return Vector2(row * other.row, column * other.column); }
	inline Vector2 operator*(const float value) const { return Vector2(row * value, column * value); }
	inline bool operator==(const Vector2& other) const { return row == other.row && column == other.column; }
	inline bool operator!=(const Vector2& other) const { return row != other.row || column != other.column; }
	inline bool operator<(const Vector2& other)const { if (row < other.row) return true; if (row > other.row) return false; return column < other.column; }

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet << column << row;
	}
	void Decode(sf::Packet& packet) override { 
		packet >> column >> row;
	}
};

struct Address : public ICodable { //Conte tota la informacio per establir una conexio amb un client
	std::string ip, name; //Ip de la conexio i nom del client
	unsigned short port; //Port de la conexio
	Address() = default;
	Address(std::string _name, std::string _ip,unsigned short _port) { name = _name; ip = _ip; port = _port; }
	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet << port << ip << name;
	}
	void Decode(sf::Packet& packet) override {
		packet >> port >> ip >> name;
	}
};

struct ChatMessage : public ICodable { //Ho enviarem als altres jugadors per enviarloshi un missatge
	//GameplayPackagesIds packetKey = GameplayPackagesIds::Message;
	ChatMessage(Address _user, std::string _message, sf::Uint64 _timeStamp, Packet& p) { user = _user; message = _message; timeStamp = _timeStamp;  Code(p); }
	ChatMessage(Address _user, std::string _message, sf::Uint64 _timeStamp) { user = _user; message = _message; timeStamp = _timeStamp; }
	ChatMessage(Packet& p) { Decode(p); }
	ChatMessage() = default;

	Address user; //Qui ha escrit el missatge
	std::string message; //El missatge
	sf::Uint64 timeStamp; //Quan sha enviat

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet << user << message << timeStamp;
	}
	void Decode(sf::Packet& packet) override {
		packet >> user >> message >> timeStamp;
	}
};

struct RoomData : public ICodable{  //Informacio que correspon a una sala
	RoomData(sf::Uint64 _id, std::string _name, sf::Uint64 _timeStamp, Packet& p) { id = _id; name = _name; timeStamp = _timeStamp; /*CreateBaseRoomIP(id);*/ Code(p); }
	RoomData(sf::Uint64 _id, std::string _name, sf::Uint64 _timeStamp) { id = _id; name = _name; timeStamp = _timeStamp; /*CreateBaseRoomIP(id);*/ }
	RoomData(sf::Uint64 _id, std::string _name) { 
		id = _id;
		//CreateBaseRoomIP(id);
		name = _name;
		// Obtener la fecha y hora actual
		std::time_t now = std::time(nullptr);
		std::tm* local_time = std::localtime(&now);
		// Construir la fecha y hora como una cadena de caracteres
		char datetime_str[20];
		std::strftime(datetime_str, sizeof(datetime_str), "%Y%m%d%H%M%S", local_time);
		// Convertir la cadena de caracteres a un sf::Uint64
		std::sscanf(datetime_str, "%llu", &this->timeStamp);
	}
	RoomData(Packet& p) { Decode(p);/* CreateBaseRoomIP(id);*/ }
	RoomData() = default;

	int playersReady = 0;
	sf::Uint64 id; //La seva id
	std::string name; //El seu nom
	sf::Uint64 timeStamp; //Data de creacio de la sala
	std::map<int, std::pair<std::string, TcpSocket*>> playersInRoom;
	std::mutex playersInRoomMutex;
	std::map<int, std::pair<std::string, TcpSocket*>> spectatorsInRoom;
	std::mutex spectatorsInRoomMutex;
	std::string baseRoomIP;
	CPVector<Address> clientsAddresses;
	std::mutex clientsAddressesMutex, clientsInRoomMutex, messagesMutex;
	int clientsInRoom = 0;
	CPList<ChatMessage> roomMessages;

	void GetAllMessages(Packet& p) { 
		CPList<ChatMessage> returnMessages;
		messagesMutex.lock();
		returnMessages = roomMessages;
		messagesMutex.unlock(); 

		p << returnMessages;
	}

	void AddMessage(ChatMessage* cm) {

		messagesMutex.lock();
		roomMessages.push_back(cm);
		messagesMutex.unlock();
	}

	//void CreateBaseRoomIP(sf::Uint64 roomId) {
	//	baseRoomIP = "192.168." + std::to_string(id + 2) + ".1"; //lultim num ha de ser minim 1 i maxim 254, rl penultim minim 2 i max 254 perque al 1 hi ha la ip del server
	//}	

	std::string GetIp(sf::Uint64 playerId) {

		// Encontrar la posición del último punto en la dirección IP
		size_t lastDotPos = baseRoomIP.find_last_of('.');
		// Extraer el último número de la dirección IP y convertirlo a entero
		int lastNum = std::stoi(baseRoomIP.substr(lastDotPos + 1));
		// Sumar 'playerId' al último número y reemplazarlo en la dirección IP
		clientsInRoomMutex.lock();
		std::string clientIP = baseRoomIP.substr(0, lastDotPos + 1) + std::to_string(lastNum + clientsInRoom);
		clientsInRoomMutex.unlock();

		return clientIP;
	}

	std::pair<bool, std::pair<bool,CPVector<Address>>> AddPlayerInRoom(std::pair<int, std::pair<std::string, TcpSocket*>> playerPair) {
		bool roomFull, isFirst;
		CPVector<Address> addresses;
		playersInRoomMutex.lock();
		isFirst = playersInRoom.empty();
		roomFull = playersInRoom.size() > 1;
		if (!roomFull) {
			clientsInRoomMutex.lock();
			clientsInRoom++;
			clientsInRoomMutex.unlock();
			playersInRoom.insert(playerPair);
			TcpSocket* sock = playerPair.second.second;
			addresses = AddAddress(playerPair.first, playerPair.second.first, sock->getRemoteAddress().toString(), sock->getLocalPort());
		}
		playersInRoomMutex.unlock();

		return std::make_pair(!roomFull, std::make_pair(isFirst, addresses));
	}

	void RemovePlayerFromRoom(int playerID) {
		playersInRoomMutex.lock();
		playersInRoom.erase(playerID);
		playersInRoomMutex.unlock();
	}

	CPVector<Address> AddSpectatorInRoom(std::pair<int, std::pair<std::string, TcpSocket*>> spectatorPair) {
		CPVector<Address> addresses;
		clientsInRoomMutex.lock();
		clientsInRoom++;
		clientsInRoomMutex.unlock();
		spectatorsInRoomMutex.lock();  
		spectatorsInRoom.insert(spectatorPair);
		TcpSocket* sock = spectatorPair.second.second;
		addresses = AddAddress(spectatorPair.first, spectatorPair.second.first, sock->getRemoteAddress().toString(), /*50001*/sock->getLocalPort());
		spectatorsInRoomMutex.unlock();
		return addresses;
	}

	void RemoveSpectatorFromRoom(int spectatorID) {
		spectatorsInRoomMutex.lock();
		spectatorsInRoom.erase(spectatorID);
		spectatorsInRoomMutex.unlock();
	}

	CPVector<Address> AddAddress(int clientID, std::string clientName, std::string ip, unsigned short port) {
		CPVector<Address> addresses;
		clientsAddressesMutex.lock();		
		clientsAddresses.push_back(new Address(clientName, ip, port));		
		addresses = clientsAddresses;
		clientsAddressesMutex.unlock();
		return addresses;
	}

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet << id << timeStamp << name;		
	}
	void Decode(sf::Packet& packet) override {
		packet >> id >> timeStamp >> name;
	}	
};

struct RoomsUpdateData : public ICodable { //Conte tota la informacio de les sales existents, es la resposta a un login
	//MatchMakingPackagesIds packetKey = MatchMakingPackagesIds::RoomsUpdate;
	RoomsUpdateData(Packet& p, bool insert){
		if (insert) {
			// Obtener la fecha y hora actual
			std::time_t now = std::time(nullptr);
			std::tm* local_time = std::localtime(&now);
			// Construir la fecha y hora como una cadena de caracteres
			char datetime_str[20];
			std::strftime(datetime_str, sizeof(datetime_str), "%Y%m%d%H%M%S", local_time);
			// Convertir la cadena de caracteres a un sf::Uint64
			sf::Uint64 timestamp;
			std::sscanf(datetime_str, "%llu", &timestamp);

			rooms.push_back(new RoomData(sf::Uint64(0), (std::string)"Default Room", timestamp));
			
			Code(p);
		}
		else
		{
			Decode(p);
		}
	}
	CPVector<RoomData> rooms; //Vector amb la informacio de cada sala

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet /*<< packetKey*/ << rooms;
	}
	void Decode(sf::Packet& packet) override {		
		packet >> rooms;
	}
};

struct LoginRequestData : public ICodable { //Peticio de login que enviarem al servidor
	//MatchMakingPackagesIds packetKey = MatchMakingPackagesIds::OnLogin;
	LoginRequestData(std::string _name, Packet& packet) { name = _name; Code(packet); }
	LoginRequestData(Packet& packet) { Decode(packet); }
	std::string name; //Nombre del jugador

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet /*<< packetKey*/ << name;
	}
	void Decode(sf::Packet& packet) override {
		packet >> name;
	}
}; 

struct CreateRoomRequestData : public ICodable { //Enviarem aixo al servidor quan volguem crear una sala
	//MatchMakingPackagesIds packetKey = MatchMakingPackagesIds::CreateRoomRequest; 
	std::string name; //Nom que haura de tenir la sala

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet /*<< packetKey*/ << name;
	}
	void Decode(sf::Packet& packet) override {
		packet >> name;
	}
};

struct CreateRoomResponseData : public ICodable { //El servidor ens enviara aixo un cop enviem que voliem crear una sala
	//MatchMakingPackagesIds packetKey = MatchMakingPackagesIds::CreateRoomResponse; 
	sf::Uint64 id; //Id de la nova sala creada que haurem de fer servir per demanar de poder unirnos

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet /*<< packetKey*/ << id;
	}
	void Decode(sf::Packet& packet) override {
		packet >> id;
	}
};

struct JoinRoomPlayerRequestData : public ICodable { //El client enviara aixo al servidor per dirli la sala a la que es vol unir com a jugador
	//MatchMakingPackagesIds packetKey = MatchMakingPackagesIds::JoinRoomPlayerRequest;
	JoinRoomPlayerRequestData(sf::Uint64 _idRoom, sf::Packet& p) { idRoom = _idRoom; Code(p); }
	sf::Uint64 idRoom; //Id de la sala a ala que ens volem unir

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet /*<< packetKey*/ << idRoom;
	}
	void Decode(sf::Packet& packet) override {
		packet >> idRoom;
	}
};

struct JoinRoomSpectatorRequestData : public ICodable { //El client enviara aixo al servidor per dirli la sala a la que es vol unir com a viewer
	//MatchMakingPackagesIds packetKey = MatchMakingPackagesIds::JoinRoomSpectatorRequest;
	JoinRoomSpectatorRequestData(sf::Uint64 _idRoom, sf::Packet& p) { idRoom = _idRoom; Code(p); }
	sf::Uint64 idRoom; //Id de la sala a ala que ens volem unir

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet /*<< packetKey*/ << idRoom;
	}
	void Decode(sf::Packet& packet) override {
		packet >> idRoom;
	}
};

    //JoinRoomPlayerResponseData 
struct EnterAsPlayerResponseData : public ICodable { //Conte tota la inforacio necesaria per conectarse amb cada client duna sala al entrar com a jugador
	//MatchMakingPackagesIds packetKey = MatchMakingPackagesIds::OnEnterAsPlayer;
	CPVector<Address> otherP2PClients; //Tota la informacio de conxeio de cada client
	bool isFirst; //Primer en entrar a la sala

	EnterAsPlayerResponseData(bool _isFirst, CPVector<Address> _otherP2PClients, Packet& p) { isFirst = _isFirst;  otherP2PClients = _otherP2PClients; Code(p); }
	EnterAsPlayerResponseData(Packet& p) { Decode(p); }
	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet/* << packetKey*/ << isFirst << otherP2PClients;
	}
	void Decode(sf::Packet& packet) override {
		packet >> isFirst >> otherP2PClients;
	}
};

	//JoinRoomSpectatorResponseData 
struct EnterAsSpectatorResponseData : public ICodable { //Conte tota la inforacio necesaria per conectarse amb cada client duna sala al entrar com a viewer
	//MatchMakingPackagesIds packetKey = MatchMakingPackagesIds::OnEnterAsSpectator;
	CPVector<Address> otherP2PClients; //Tota la informacio de conxeio de cada client

	EnterAsSpectatorResponseData(CPVector<Address> _otherP2PClients, Packet& p) { otherP2PClients = _otherP2PClients; Code(p); }
	EnterAsSpectatorResponseData(Packet& p) { Decode(p); }
	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet /*<< packetKey*/ << otherP2PClients;
	}
	void Decode(sf::Packet& packet) override {
		packet >> otherP2PClients;
	}
};

struct EnterAsPlayerResponseErrorData : public ICodable { //Serveix per informar que sens han avançat a l'hora dentrar a la sala i la sala es plena
	//MatchMakingPackagesIds packetKey = MatchMakingPackagesIds::OnEnterAsPlayerFailure;

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		//packet << packetKey;
	}
	void Decode(sf::Packet& packet) override { }
};

struct EnterAsSpectatorResponseErrorData : public ICodable { //Encara no te us, pero es crea per si fos necesari
	//MatchMakingPackagesIds packetKey = MatchMakingPackagesIds::OnEnterAsSpectatorFailure;

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		//packet << packetKey;
	}
	void Decode(sf::Packet& packet) override { } 
};

struct ExitRoomRequestData : public ICodable { //Informacioque enviarem al server sobre quina sala volem abandonar
	//MatchMakingPackagesIds packetKey = MatchMakingPackagesIds::ExitRoom;
	sf::Uint64 idRoom; //id de la sala q volem abandonar

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet /*<< packetKey*/ << idRoom;
	}
	void Decode(sf::Packet& packet) override {
		packet >> idRoom;
	}
};

struct PlayerReadyResponseData : public ICodable { //Ho enviarem al server per informar que hem entrar correctament com a jugador a la partida i estem preparats per començar
	//MatchMakingPackagesIds packetKey = MatchMakingPackagesIds::SetReady;

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		//packet << packetKey;
	}
	void Decode(sf::Packet& packet) override { }
};

struct GameStartResponseData : public ICodable { //Ho enviara el servidor al client quan hagi rebut de cada jugador el isReady perque comenci la partida
	//MatchMakingPackagesIds packetKey = MatchMakingPackagesIds::GameStart; //al rebre aixo els clients s'han de desconectar del servidor

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		//packet << packetKey;
	}
	void Decode(sf::Packet& packet) override { }
};

struct SelectPositionData : public ICodable { //Ho enviarem als altres clients per infromar de la piece que tenim seleccionada
	//GameplayPackagesIds packetKey = GameplayPackagesIds::SelectPiece;
	Vector2 selectedPosition; //Posicio seleccionada al tablero

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet /*<< packetKey*/ << selectedPosition;
	}
	void Decode(sf::Packet& packet) override {
		packet >> selectedPosition;
	}
};

struct DeselectPositionData : public ICodable { //Ho enviarem als altres clients per infromar que deseleccionem la ultima peça que teniem seleccionada
	//GameplayPackagesIds packetKey = GameplayPackagesIds::Deselect;	

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		//packet << packetKey;
	}
	void Decode(sf::Packet& packet) override { }
};

struct MoveToPositionData : public ICodable { //Ho enviarem a altres clients per informar de la nova posicio que tindra la ultima ficha seleccionada
	//GameplayPackagesIds packetKey = GameplayPackagesIds::MovePiece;
	Vector2 posToMove; //Nova posicio de la ficha

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet/* << packetKey*/ << posToMove;
	}
	void Decode(sf::Packet& packet) override {
		packet >> posToMove; 
	}
};

struct PromotionData : public ICodable {  //Ho enviarem per indicar la nova posicio dun peo que pot fer promotion i la nova fitxa en la que es convertira
	//GameplayPackagesIds packetKey = GameplayPackagesIds::Promotion;
	Vector2 posToMove; //Nova posicio del peo
	PieceType piece;  //Nova fitxa en la que es convertira

	PromotionData(Packet& p, bool code) { if (code) Code(p); else Decode(p); }

	PromotionData(Vector2 _posToMove, PieceType _piece, Packet& p) : posToMove(_posToMove), piece(_piece) { Code(p); }
	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet << posToMove << static_cast<sf::Uint8>(piece);
	}
	void Decode(sf::Packet& packet) override {
		sf::Uint8 uintPieceType;
		packet >> posToMove >> uintPieceType;
		piece = static_cast<PieceType>(uintPieceType);
	}
};

struct CastleData : public ICodable { //Ho enviarem als altre clients per informar que el rei i una torre que indiquem han fet un enroc
	//GameplayPackagesIds packetKey = GameplayPackagesIds::Castle;
	Vector2 kingPositionToMove; //Nova posicio del rei
	Vector2 towerPositionToMove; //Nova posicio de la torre
	Vector2 towerOriginPosition; //Posicio on es trobava originalment la torre

	CastleData(Packet& p, bool code) { if (code) Code(p); else Decode(p); }
	CastleData(Vector2 _kingPositionToMove, Vector2 _towerPositionToMove,Vector2 _towerOriginPosition, Packet& p) : kingPositionToMove(_kingPositionToMove), towerPositionToMove(_towerPositionToMove), towerOriginPosition(_towerOriginPosition){ Code(p); }

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet << kingPositionToMove << towerPositionToMove << towerOriginPosition;
	}
	void Decode(sf::Packet& packet) override {
		packet >> kingPositionToMove >> towerPositionToMove >> towerOriginPosition;
	}
};

struct EnPassantData : public ICodable { //Ho enviarem a altres clients per informar de la nova posicio del peo que ha fet el enPassant, y la posicio que tenia el peo que ha mort
	//GameplayPackagesIds packetKey = GameplayPackagesIds::EnPassant;
	Vector2 positionToMove; //Nova poisico del peo que mata
	Vector2 pawnKilledPosition; //Posicio on es trobava el peo que ha mort

	EnPassantData(Packet& p, bool code) { if (code) Code(p); else Decode(p); }
	EnPassantData(Vector2 _positionToMove, Vector2 _pawnKilledPosition, Packet& p) : positionToMove(_positionToMove), pawnKilledPosition(_pawnKilledPosition) { Code(p); }

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override {
		packet << positionToMove << pawnKilledPosition;
	}
	void Decode(sf::Packet& packet) override {
		packet >> positionToMove >> pawnKilledPosition;
	}
};



struct ChatMessagesRequest : public ICodable { //Ho enviarem al Player1 o Server per solicitar tots els missatges de la sala
	//GameplayPackagesIds packetKey = GameplayPackagesIds::MessagesRequest;

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override { }
	void Decode(sf::Packet& packet) override { }
};

struct ChatMessagesResponse : public ICodable { //Ho enviarem al jugador que hagi solicitat el registre de missatges
	//GameplayPackagesIds packetKey = GameplayPackagesIds::MessagesResponse;
	ChatMessagesResponse(Packet& p) { Decode(p); }
	ChatMessagesResponse() = default;
	CPList<ChatMessage> messages; //Llista amb la info de cada missatge

	// Heredado vía ICodable
	void Code(sf::Packet& packet) override { 
		//packet << messages;
	}
	void Decode(sf::Packet& packet) override { 
		packet >> messages;
	}
};

struct SelectAppModeManager {
	SelectAppModeManager(sf::RenderWindow* appWindow, sf::Font font, bool* modeisServer);
};

struct ServerStateManager {
	ServerStateManager(SocketsManager* sM, sf::RenderWindow* appWindow, sf::Font font, sf::IpAddress* ipAddress, bool* serverState, unsigned short port);
	bool TurnServerUp(SocketsManager* sM, sf::IpAddress* ipAddress, unsigned short port);
	bool TurnServerDown(SocketsManager* sM);
};

struct LogInManager {
	LogInManager(SocketsManager* sM, std::string* name, sf::RenderWindow* appWindow, sf::Font font, std::atomic<ClientState>* clientState);
};


struct LobbyManager {
	LobbyManager(TcpSocket* socket, sf::RenderWindow* appWindow, sf::Font font, RoomsUpdateData* lobbyInfo, std::atomic<ClientState>* clientState, std::mutex* lobbyMutex, std::atomic <sf::Uint64>*seed);

	void RePositionUi(sf::RenderWindow* appWindow, sf::Font font, RoomsUpdateData* lobbyInfo, std::mutex* lobbyMutex, 
		std::map<sf::Int64, std::pair<sf::RectangleShape, sf::RectangleShape>>& roomsButtons, std::pair<sf::RectangleShape, sf::RectangleShape>& newRoom, bool showExtraRoom);
	std::string GetDate(sf::Uint64 numDate);
};

struct ChatManager {
	sf::RenderWindow* gameWindow;
	SocketsManager* sM;
	std::string input;
	std::string name;
	std::mutex processedMssgsMutex;
	std::multimap<sf::Uint64, std::string> processedMssgs;
	std::vector<sf::RectangleShape>chatUI;
	sf::Text sfInputText, chatTitle;
	sf::Font font;
	sf::RectangleShape* backGround;
	sf::RectangleShape* backGroundTitle;
	sf::RectangleShape* backGroundInput;
	TcpSocket* serverSocket;

	ChatManager(sf::RenderWindow* _gameWindow, SocketsManager* _sM, std::string _name, sf::Font _font, TcpSocket* _serverSocket);
	void ProcessMssgs();
	void HandleInput(sf::Event event);
	void HandleInput();
	void LoadMessage(ChatMessage mssg);
	void LoadMessageList(CPList<ChatMessage> messages);

	/*sf::Uint64 GetTimeSinceSended(sf::Uint64 timeStamp);*/
	std::string GetMssgTime(sf::Uint64 timeStamp);
	sf::Uint64 GetTime();
};