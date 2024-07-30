#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <set>
#include <unordered_map>
#include "Structures.h"

class BoardSquare;
class ChessPiece;
class King;
class Pawn;
class SocketsManager;

class ChessManager
{
public:
    static ChessManager* GetInstace();
    void RunGame(sf::RenderWindow* window, std::atomic<ClientState>* _currentClientState, std::atomic <sf::Uint64>* seed, 
        bool isFirst, std::atomic <bool>* _gameReady, SocketsManager*_sM, CPVector<Address> _p2pAdresses, TcpSocket* _serverSocket, sf::Font _font, std::string clientName);
    void MessageReceived(ChatMessage message);
    void LoadMessageHistory(CPList<ChatMessage> messages);
    void PieceSelected(Vector2 posPiece);
    void PieceDeselected();
    void PieceMove(Vector2 movePos);
    void EnPasssantMove(EnPassantData data);
    void PromotionMove(PromotionData data);
    void CastleMove(CastleData data);
    ~ChessManager();
private:  
    static ChessManager* instance;
    ChatManager* chatManager;
    bool whiteMoves = true, gameEnded = false;
    GAMESTATE currentGS;
    std::atomic<ClientState>* currentClientState;
    //std::atomic <sf::Uint64>* seedForRandomColor;
    Vector2 newKingPos;
    BoardSquare* board[ChessSideLength][ChessSideLength];
    BoardSquare* selectedSquare = nullptr;
    std::map<Vector2, ChessPiece*>piecesAlive;
    std::vector<std::pair<bool, std::set<Vector2>>> pathsToCheckingKing;
    King* blackKing, * whiteKing;
    Pawn* enPassantPawn;
    sf::Sprite spriteSheet;
    sf::Texture textureSheet;
    sf::RenderWindow* gameWindow;
    sf::Text errorText;
    sf::Font font;

    bool p2pDone = false, isFirst;

    sf::RectangleShape* gameOffEffect, *readyButton;
    sf::Text readyText;
    bool clientIsWhite;
    std::atomic <bool>* gameReady;
    SocketsManager* sM;
    CPVector<Address> p2pAdresses;
    TcpSocket* serverSocket;

    bool canTypeInChat = true, showReadyButton = true;
    sf::RectangleShape* backGroundBottom;

    Vector2* selectedPiecePos;

    ChessManager();
    void Update();
    void Render();
    void FinalRender();

    void ColorsHover(sf::Vector2i mousePos);
    void ColorsSelection();
    void ColorChecks();
    void ColorSpecialMoves(ChessPiece* piece);

    void CheckMoves(sf::Vector2i mousePos);
    void HandleSpecialMoves(SpecialMove& sMove, Vector2 pos);

    void CheckKingsChecks();
    bool CheckIfKingVulnerable(Vector2 newPos, ChessPiece* selectedPiece, bool printWarningText);
    void CheckMate(King* selectedKing);

    void HandleEnPassant(Vector2 newPos, bool performed);
    void HandleCastling(Vector2 newPos, bool performed);
    void HandlePromotion(SpecialMove& sMove, Vector2 newPos);
    Vector2 CanKillEnPassant(ChessPiece* piece);
    Vector2 CanPerformCastling(ChessPiece* piece, ChessPiece* kingTarget, Vector2 pos);
    SpecialMove CanDoSpecialMove(ChessPiece* piece, Vector2 selectedPos, bool* targetIsCorrect);

    void LoadPiece(Vector2 keyPos, ChessPiece* piece);
    void GenerateColorFromSeed(sf::Uint64 seed, bool isFirst);

    void StartP2P();
    void StartListener(unsigned short port, std::string ip);
    void CheckIfReady(sf::Vector2i mousePos);
};

