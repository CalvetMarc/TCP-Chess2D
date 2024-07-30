#include "ChessManager.h"
#include "BoardSquare.h"
#include "ChessPiece.h"
#include "SocketsManager.h"
#include <algorithm>
#include <set>
#include <random>

ChessManager* ChessManager::instance = nullptr;

ChessManager::ChessManager()
{    
    backGroundBottom = new sf::RectangleShape(sf::Vector2f(GAMEWIDTH, TOTALHEIGTH - GAMEHEIGTH));
    backGroundBottom->setOrigin(backGroundBottom->getGlobalBounds().width * 0.5f, backGroundBottom->getGlobalBounds().height * 0.5f);
    backGroundBottom->setPosition(GAMEWIDTH * 0.5f, TOTALHEIGTH - 25);
    backGroundBottom->setFillColor(sf::Color(200, 200, 180));
    currentGS = GAMESTATE::PLAYING;
    errorText.setFont(font); // Establecer la fuente
    errorText.setCharacterSize(16); // Establecer el tamaño del texto
    errorText.setPosition(GAMEWIDTH / 2, GAMEHEIGTH);
    errorText.setFillColor(sf::Color::Red);
    textureSheet.loadFromFile("resources/Piezas.png");
    spriteSheet.setTexture(textureSheet);

    for (int i = 0; i < ChessSideLength; ++i) {
        for (int j = 0; j < ChessSideLength; ++j) {
            // Si la suma de i y j es par, la casilla debe ser blanca; de lo contrario, debe ser negra
            if ((i + j) % 2 == 0) {
                board[i][j] = new BoardSquare(sf::Color::White, Vector2(i, j));
            }
            else {
                board[i][j] = new BoardSquare(sf::Color(156, 124, 73), Vector2(i, j));
            }
        }
    }

    //Creem totes les peces i les coloquem
    LoadPiece(Vector2(0, 0), new Rook(false, spriteSheet, PieceType::RookType)); //Black Rook Left
    LoadPiece(Vector2(0, 7), new Rook(false, spriteSheet, PieceType::RookType)); //Black Rook Right    
    LoadPiece(Vector2(0, 1), new Knight(false, spriteSheet, PieceType::KnightType)); //Black Knight Left    
    LoadPiece(Vector2(0, 6), new Knight(false, spriteSheet, PieceType::KnightType)); //Black Knight Right    
    LoadPiece(Vector2(0, 2), new Bishop(false, spriteSheet, PieceType::BishopType)); //Black Bishop Left    
    LoadPiece(Vector2(0, 5), new Bishop(false, spriteSheet, PieceType::BishopType)); //Black Bishop Right    
    LoadPiece(Vector2(0, 3), new Queen(false, spriteSheet, PieceType::QueenType)); //Black Queen   
    LoadPiece(Vector2(0, 4), new King(false, spriteSheet, PieceType::KingType)); //Black King   
    blackKing = dynamic_cast<King*>(piecesAlive[Vector2(0, 4)]);

    LoadPiece(Vector2(7, 0), new Rook(true, spriteSheet, PieceType::RookType)); //White Rook Left
    LoadPiece(Vector2(7, 7), new Rook(true, spriteSheet, PieceType::RookType)); //White Rook Right    
    LoadPiece(Vector2(7, 1), new Knight(true, spriteSheet, PieceType::KnightType)); //White Knight Left    
    LoadPiece(Vector2(7, 6), new Knight(true, spriteSheet, PieceType::KnightType)); //White Knight Right    
    LoadPiece(Vector2(7, 2), new Bishop(true, spriteSheet, PieceType::BishopType)); //White Bishop Left    
    LoadPiece(Vector2(7, 5), new Bishop(true, spriteSheet, PieceType::BishopType)); //White Bishop Right    
    LoadPiece(Vector2(7, 3), new Queen(true, spriteSheet, PieceType::QueenType)); //White Queen   
    LoadPiece(Vector2(7, 4), new King(true, spriteSheet, PieceType::KingType)); //White King  
    whiteKing = dynamic_cast<King*>(piecesAlive[Vector2(7, 4)]);

    for (int i = 0; i < ChessSideLength; i++) {
        LoadPiece(Vector2(1, i), new Pawn(false, spriteSheet, PieceType::PawnType)); //Black Pawn
        LoadPiece(Vector2(6, i), new Pawn(true, spriteSheet, PieceType::PawnType)); //White Pawn
    }
}

void ChessManager::Update()
{
    while (gameWindow->isOpen() && currentGS  == GAMESTATE::PLAYING) //Mentres no tanquem la finestra juguem
    { 
        sf::Vector2i mousePos = sf::Mouse::getPosition(*gameWindow);


        if (*gameReady) {
            /*if (!p2pDone)
                StartP2P();*/
            
            ColorChecks();  //Colors fichas que tenen en jaque un rey
            ColorsSelection(); //Colors ficha seleccionada
            if (mousePos.x > 0 && mousePos.x < GAMEWIDTH && mousePos.y > 0 && mousePos.y < GAMEHEIGTH) { //Si el ratoli esta dins la zona de joc comprovem quina posicions s'han de marcar com correctes i quina ficha estem seleccionant si ho fem
                ColorsHover(mousePos);  //Colors ficha que passem per sobre                
                CheckMoves(mousePos);                
            }
            else {
                chatManager->HandleInput();
            }            
            
        } 
        else {
            CheckIfReady(mousePos);
        }

        Render();      

        if (currentGS != GAMESTATE::PLAYING)
            FinalRender();
    }    
    system("pause");
    int a = 10;
}

ChessManager* ChessManager::GetInstace()
{
    if (instance == nullptr)
        instance = new ChessManager();
    
    return instance;
}

void ChessManager::RunGame(sf::RenderWindow* window, std::atomic<ClientState>* _currentClientState, std::atomic <sf::Uint64>* seed, bool _isFirst, std::atomic <bool>* _gameReady, 
    SocketsManager* _sM, CPVector<Address> _p2pAdresses, TcpSocket* _serverSocket, sf::Font _font, std::string clientName)
{
    gameWindow = window;


    font = _font;
    gameWindow->setSize(sf::Vector2u(TOTALWIDTH, TOTALHEIGTH));
    gameWindow->setView(sf::View(sf::FloatRect(0, 0, TOTALWIDTH, TOTALHEIGTH)));
    std::string textT = *_currentClientState == ClientState::PLAYER ? "OTHER PLAYER READY" : "PLAYERS READY";
    gameWindow->setTitle("CHESS - WAITING " + textT);
    gameOffEffect = new sf::RectangleShape(sf::Vector2f(GAMEWIDTH, GAMEHEIGTH));
    gameOffEffect->setPosition(sf::Vector2f(0, 0));
    gameOffEffect->setFillColor(sf::Color(128, 128, 128, 160));
    readyButton = new sf::RectangleShape(sf::Vector2f(140, 35));
    readyButton->setOrigin(readyButton->getGlobalBounds().width * 0.5f, readyButton->getGlobalBounds().height * 0.5f);
    readyButton->setPosition(sf::Vector2f(GAMEWIDTH * 0.5f, GAMEHEIGTH + 25));
    readyButton->setFillColor(sf::Color(0, 160, 0));
    readyText.setString("PRESS IF READY");
    readyText.setCharacterSize(14);
    readyText.setOrigin(readyText.getGlobalBounds().width * 0.5f, readyText.getGlobalBounds().height * 0.5f);
    readyText.setPosition(readyButton->getPosition() - sf::Vector2f(60, 10));
    readyText.setFillColor(sf::Color::White);
    readyText.setFont(font);

    chatManager = new ChatManager(gameWindow, _sM, clientName, font, _serverSocket);
    isFirst = _isFirst;

    currentClientState = _currentClientState;
    gameReady = _gameReady;
    serverSocket = _serverSocket;
    sM = _sM;
    p2pAdresses = _p2pAdresses;
    if (*currentClientState == ClientState::PLAYER) {
        GenerateColorFromSeed(*seed, _isFirst);
    }

    _serverSocket->Send(MessagesRequest, Packet());

    Update();
}

void ChessManager::MessageReceived(ChatMessage message)
{
    chatManager->LoadMessage(message);
}

void ChessManager::LoadMessageHistory(CPList<ChatMessage> messages)
{
    chatManager->LoadMessageList(messages);
}

void ChessManager::PieceSelected(Vector2 posPiece)
{
    selectedPiecePos = new Vector2(posPiece);
}

void ChessManager::PieceDeselected()
{
    delete selectedPiecePos;
    selectedPiecePos = nullptr;
}

void ChessManager::PieceMove(Vector2 movePos)
{
    errorText.setString("");
    ChessPiece* pieceOnTarget = board[movePos.row][movePos.column]->GetPiece();
    if (pieceOnTarget != nullptr) {
        delete piecesAlive[Vector2(movePos.row, movePos.column)];  //Borrem la peça que hem matat de la partida y actualitzem al vector de peces la nova posicio de la que hem mogut                            
    }

    //HandleSpecialMoves(smResult, Vector2(row, col));
    ChessPiece* pieceSelected = board[selectedPiecePos->row][selectedPiecePos->column]->GetPiece();
    piecesAlive.erase(board[selectedPiecePos->row][selectedPiecePos->column]->GetPiece()->GetPos());    //actualitzem el moviment
    pieceSelected->SetFirstMoveDone(true);

    if (enPassantPawn != nullptr) {
        enPassantPawn = nullptr;
    }

    if ( Pawn*p = dynamic_cast<Pawn*>(pieceSelected)) {
        if (std::abs(movePos.row - selectedPiecePos->row) == 2) {
            enPassantPawn = p;
        }
    }

    piecesAlive[Vector2(movePos.row, movePos.column)] = pieceSelected;
    board[selectedPiecePos->row][selectedPiecePos->column]->SetPiece(nullptr);
    board[movePos.row][movePos.column]->SetPiece(pieceSelected);
    whiteMoves = !whiteMoves;  //Canviem el torn 
    delete selectedPiecePos;
    selectedPiecePos = nullptr;

    CheckKingsChecks(); //Comprovem els camins de les peces al jaque per pintarlos i si es un jaque mate
}

void ChessManager::EnPasssantMove(EnPassantData data)
{
    //HandleSpecialMoves(smResult, Vector2(row, col));
    ChessPiece* pieceSelected = board[selectedPiecePos->row][selectedPiecePos->column]->GetPiece();
    piecesAlive.erase(board[selectedPiecePos->row][selectedPiecePos->column]->GetPiece()->GetPos());    //actualitzem el moviment
    pieceSelected->SetFirstMoveDone(true);
    if (Pawn* p = dynamic_cast<Pawn*>(pieceSelected)) {
        board[data.pawnKilledPosition.row][data.pawnKilledPosition.column]->SetPiece(nullptr);
        piecesAlive.erase(data.pawnKilledPosition);
    }

    piecesAlive[Vector2(data.positionToMove.row, data.positionToMove.column)] = pieceSelected;
    board[selectedPiecePos->row][selectedPiecePos->column]->SetPiece(nullptr);
    board[data.positionToMove.row][data.positionToMove.column]->SetPiece(pieceSelected);
    whiteMoves = !whiteMoves;  //Canviem el torn 
    delete selectedPiecePos;
    selectedPiecePos = nullptr;
    if (enPassantPawn != nullptr) {
        delete enPassantPawn;
        enPassantPawn = nullptr;
    }

    CheckKingsChecks(); //Comprovem els camins de les peces al jaque per pintarlos i si es un jaque mate
}

void ChessManager::PromotionMove(PromotionData data)
{
    ChessPiece* pieceOnTarget = board[data.posToMove.row][data.posToMove.column]->GetPiece();
    if (pieceOnTarget != nullptr) {
        delete piecesAlive[Vector2(data.posToMove.row, data.posToMove.column)];  //Borrem la peça que hem matat de la partida y actualitzem al vector de peces la nova posicio de la que hem mogut                            
    }     

    switch (static_cast<PieceType>(data.piece))
    {
    case PieceType::KnightType:
        piecesAlive[Vector2(data.posToMove.row, data.posToMove.column)] = new Knight(whiteMoves, spriteSheet, data.piece);
        break;
    case PieceType::BishopType:
        piecesAlive[Vector2(data.posToMove.row, data.posToMove.column)] = new Bishop(whiteMoves, spriteSheet, data.piece);
        break;
    case PieceType::RookType:
        piecesAlive[Vector2(data.posToMove.row, data.posToMove.column)] = new Rook(whiteMoves, spriteSheet, data.piece);
        break;
    case PieceType::QueenType:
        piecesAlive[Vector2(data.posToMove.row, data.posToMove.column)] = new Queen(whiteMoves, spriteSheet, data.piece);
        break;
    default:
        break;
    }

    board[data.posToMove.row][data.posToMove.column]->SetPiece(piecesAlive[Vector2(data.posToMove.row, data.posToMove.column)]);
    delete piecesAlive[Vector2(selectedPiecePos->row, selectedPiecePos->column)];
    piecesAlive.erase(Vector2(selectedPiecePos->row, selectedPiecePos->column));
    board[selectedPiecePos->row][selectedPiecePos->column]->SetPiece(nullptr);
    delete selectedPiecePos;
    selectedPiecePos = nullptr;

    if (enPassantPawn != nullptr) {
        enPassantPawn = nullptr;
    }

    whiteMoves = !whiteMoves;
}

void ChessManager::CastleMove(CastleData data)
{
    
    Vector2 originTowerCastlePos;
    originTowerCastlePos.column = data.kingPositionToMove.column < 4 ? 0 : 7;
    originTowerCastlePos.row = data.kingPositionToMove.row;

    Vector2 originKingCastlePos;
    originKingCastlePos.column = 4;
    originKingCastlePos.row = data.kingPositionToMove.row;
    
    piecesAlive.erase(Vector2(originTowerCastlePos.row, originTowerCastlePos.column)); //Borrem del vector on estava la torre
    piecesAlive[Vector2(data.towerPositionToMove.row, data.towerPositionToMove.column)] = board[originTowerCastlePos.row][originTowerCastlePos.column]->GetPiece(); //Posem la torre al vector a la seva nova posicio
    board[originTowerCastlePos.row][originTowerCastlePos.column]->SetPiece(nullptr); //On estava la torre al tablero ara no hi ha cap peça
    board[data.towerPositionToMove.row][data.towerPositionToMove.column]->SetPiece(piecesAlive[Vector2(data.towerPositionToMove.row, data.towerPositionToMove.column)]);//Nova posicio de la torre al tablero

    piecesAlive.erase(Vector2(originKingCastlePos.row, originKingCastlePos.column)); //Borrem del vector on estava la torre
    piecesAlive[Vector2(data.kingPositionToMove.row, data.kingPositionToMove.column)] = board[originKingCastlePos.row][originKingCastlePos.column]->GetPiece(); //Posem el rei al vector a la seva nova posicio
    board[originKingCastlePos.row][originKingCastlePos.column]->SetPiece(nullptr); //On estava el rei al tablero ara no hi ha cap peça
    board[data.kingPositionToMove.row][data.kingPositionToMove.column]->SetPiece(piecesAlive[Vector2(data.kingPositionToMove.row, data.kingPositionToMove.column)]);//Nova posicio del rei al tablero

    if (enPassantPawn != nullptr) {
        enPassantPawn = nullptr;
    }

    whiteMoves = !whiteMoves;
}

ChessManager::~ChessManager()
{
    for (int i = 0; i < ChessSideLength; ++i) {
        for (int j = 0; j < ChessSideLength; ++j) {
            delete board[i][j];
            board[i][j] = nullptr; // Opcional: establecer el puntero a nullptr después de eliminarlo
        }
    }

    instance = nullptr;
}

void ChessManager::ColorsHover(sf::Vector2i mousePos)
{
    if (selectedSquare == nullptr) { //Si no tenim una ficha seleccionada
        int col = mousePos.x / (GAMEWIDTH / 8); //Casella a la cual tenim a sobre el ratoli
        int row = mousePos.y / (GAMEWIDTH / 8);

        ChessPiece* p = board[row][col]->GetPiece(); //Si la casella no te ficha no cal seguir mirant
        if (p == nullptr)
            return;

        if (*currentClientState == ClientState::PLAYER) {
            if (p->GetIsWhite() != clientIsWhite)
                return;
        }        

        std::set<Vector2>movesAvailable = p->GetCorrectMoves(piecesAlive);

        for (auto it = movesAvailable.begin(); it != movesAvailable.end(); it++) {
            board[it->row][it->column]->SetCurrentColor(sf::Color::Red); //Pintem en vermell tots els moviments disponibles d'aquella ficha
        }

        ColorSpecialMoves(p);

        board[row][col]->SetCurrentColor(sf::Color::Blue); //Pintem en blau la casella de la ficha que estem assobre        
    }    
    
}

void ChessManager::ColorSpecialMoves(ChessPiece* piece) {
    std::vector<Vector2> specialPosToColor;
    Vector2 killPos;
    switch (piece->GetType())
    {
    case PieceType::PawnType:
        killPos = CanKillEnPassant(piece);
        if(killPos.row >= 0)
            specialPosToColor.push_back(killPos); //Si es pot fer enpassant passem la posicio a les que s'han de pintar com especials    
        break;
    case PieceType::RookType:
    case PieceType::KingType:
        int row = piece->GetIsWhite() ? 7 : 0;
        ChessPiece* kingToMove = piece->GetIsWhite() ? whiteKing : blackKing;
        bool pieceIsKing = piece == kingToMove;

        Vector2 newKingLeftPos = CanPerformCastling(piece, kingToMove, pieceIsKing ? Vector2(row, 0) : kingToMove->GetPos()); //mirem si es pot fer enroque amb la torre esquerra
        if (newKingLeftPos.row >= 0)
            pieceIsKing ? specialPosToColor.push_back(Vector2(row, 0)) : specialPosToColor.push_back(kingToMove->GetPos()); //si es posible passem la posicio del rei o torre al vector per pintarles especials

        Vector2 newKingRightPos = CanPerformCastling(piece, kingToMove, pieceIsKing ? Vector2(row, 7) : kingToMove->GetPos()); //mirem si es pot fer enroque amb la torre esquerra
        if (newKingRightPos.row >= 0)
            pieceIsKing ? specialPosToColor.push_back(Vector2(row, 7)) : specialPosToColor.push_back(kingToMove->GetPos()); //si es posible passem la posicio del rei o torre al vector per pintarles especials

        break;
    }

    for (int i = 0; i < specialPosToColor.size(); i++) {
        board[specialPosToColor[i].row][specialPosToColor[i].column]->SetCurrentColor(sf::Color::Yellow); //Totes les posicions especials disponibles es pinten de groc
    }
}

void ChessManager::ColorChecks()
{
    for (auto it = pathsToCheckingKing.begin(); it != pathsToCheckingKing.end(); it++) {
        for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            board[it2->row][it2->column]->SetCurrentColor(sf::Color::Magenta);  //Tots els camins que porten a un jaque es pinten liles
        }
    }
}

Vector2 ChessManager::CanKillEnPassant(ChessPiece* piece) {
    if (enPassantPawn == nullptr)
        return Vector2(-1, -1);

    Vector2 enPPpos = enPassantPawn->GetPos();
    Vector2 pawnPos = piece->GetPos();

    if (Pawn* pawnKiller = dynamic_cast<Pawn*>(piece)) {
        if (pawnKiller->GetIsWhite() != enPassantPawn->GetIsWhite() && enPPpos.row == pawnPos.row && std::abs(enPPpos.column - pawnPos.column) == 1) { //Si estem a la seguent columna i mateixa fila que un peo que ha fet doble moviment podem fer enpassant
            int sign = enPassantPawn->GetIsWhite() ? 1 : -1;
            Vector2 killPos = enPassantPawn->GetPos() + Vector2(sign, 0);
            return killPos;
        }
    }

    return Vector2(-1, -1);
}

void ChessManager::ColorsSelection()
{
    if (selectedSquare != nullptr) {
        ChessPiece* p = selectedSquare->GetPiece();
        std::set<Vector2>movesAvailable = p->GetCorrectMoves(piecesAlive);

        for (auto it = movesAvailable.begin(); it != movesAvailable.end(); it++) { //Pintem en vermell tots els moviments disponibles d'aquella ficha
            board[it->row][it->column]->SetCurrentColor(sf::Color::Red);
        }

        ColorSpecialMoves(p);

        selectedSquare->SetCurrentColor(sf::Color::Green); //Pintem de verd la casella de la ficha que tenim seleccionada
    }
}

void ChessManager::Render()
{
    gameWindow->clear(); //Borra el que havia pintat a la finestra del frame anterior

    chatManager->ProcessMssgs();
    gameWindow->draw(*backGroundBottom);

    for (int i = 0; i < ChessSideLength; i++) { //Pintar cada casella del tablero i la seva ficha si en te
        for (int j = 0; j < ChessSideLength; j++) {
            board[i][j]->Draw(gameWindow);
        }
    }
    gameWindow->draw(errorText);

    if (currentGS == GAMESTATE::PLAYING) {
        if (!*gameReady)
            gameWindow->draw(*gameOffEffect);
        if (*currentClientState == ClientState::PLAYER && showReadyButton) {
            gameWindow->draw(*readyButton);
            gameWindow->draw(readyText);
        }
    }    

    gameWindow->display(); //Que es vegi a la finestra el que hem pintat
}

void ChessManager::FinalRender()
{
    ColorChecks();
    Render();
    //system("pause");
}

void ChessManager::CheckMoves(sf::Vector2i mousePos)
{
    int col = mousePos.x / (GAMEWIDTH / 8);
    int row = mousePos.y / (GAMEWIDTH / 8);

    sf::Event event;
    while (gameWindow->pollEvent(event))
    {
        if (event.type == sf::Event::Closed) { //Si donem a la creu de la finestra aquesta es tanca
            gameWindow->close();
        }
        if (*currentClientState == ClientState::PLAYER && clientIsWhite == whiteMoves){

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) { //Si fem click izquierdo
                if (selectedSquare == nullptr) {        
                    ChessPiece* p = board[row][col]->GetPiece();        //Si no teniem cap casella seleccionada i la casella que estem seleccionant te una ficha i es del color del torn que toca, la seleccionem
                    if (p != nullptr && p->GetIsWhite() == whiteMoves) {
                        selectedSquare = board[row][col];
                        errorText.setString(" ");
                        Vector2 pos(row, col);
                        Packet p;
                        pos.Code(p);
                        serverSocket->Send(SelectPiece, p);
                    }
                }
                else { //Si teniem una casella seleccionada

                    if (selectedSquare == board[row][col]) { //Si seleccionem la mateixa casella la deseleccionem
                        selectedSquare = nullptr;
                        Packet p;
                        serverSocket->Send(Deselect, p);
                    }
                    else {
                        bool targetIsCorrect = false;                    
                        std::set<Vector2>movesAvailable = selectedSquare->GetPiece()->GetCorrectMoves(piecesAlive); //Mirem si la casella que seleccionem per mourens es una de les posicions disponibles de la ficha que esta a la casella que teniem seleccioanada
                    
                        for (auto it = movesAvailable.begin(); it != movesAvailable.end(); it++)
                        {
                            targetIsCorrect |= *it == Vector2(row, col);
                        }

                        SpecialMove smResult = CanDoSpecialMove(selectedSquare->GetPiece(), Vector2(row, col), &targetIsCorrect);
                    
                        if (targetIsCorrect && (smResult == SpecialMove::CASTLING || !CheckIfKingVulnerable(Vector2(row, col), selectedSquare->GetPiece(), true))) {  //Si es una posicio valida i no deixa el propi rei en jaque o ya hem revisat que es un enroque
                            errorText.setString("");
                            ChessPiece* pieceOnTarget = board[row][col]->GetPiece();
                            if (smResult != SpecialMove::CASTLING && pieceOnTarget != nullptr) {
                                delete piecesAlive[Vector2(row, col)];  //Borrem la peça que hem matat de la partida y actualitzem al vector de peces la nova posicio de la que hem mogut                            
                            }                        

                            HandleSpecialMoves(smResult, Vector2(row, col));

                            if (smResult != SpecialMove::CASTLING) {
                                piecesAlive.erase(selectedSquare->GetPiece()->GetPos());    //actualitzem el moviment
                                piecesAlive[Vector2(row, col)] = selectedSquare->GetPiece();
                                board[row][col]->SetPiece(selectedSquare->GetPiece());
                                selectedSquare->SetPiece(nullptr);
                            }

                            if (smResult == SpecialMove::NONE) {
                                Vector2 pos(row, col);
                                Packet p;
                                pos.Code(p);
                                serverSocket->Send(MovePiece, p);
                            }                            

                            selectedSquare = nullptr;
                            whiteMoves = !whiteMoves;  //Canviem el torn 
                            CheckKingsChecks(); //Comprovem els camins de les peces al jaque per pintarlos i si es un jaque mate
                        
                        }       
                    }                
                }
            }
            else {
                chatManager->HandleInput(event);
            }
        }
        else {
            chatManager->HandleInput(event);
        }
    }

}

void ChessManager::HandleSpecialMoves(SpecialMove& sMove, Vector2 pos) {
    HandleEnPassant(pos, sMove == SpecialMove::ENPASSANT);
    HandleCastling(newKingPos, sMove == SpecialMove::CASTLING);
    HandlePromotion(sMove, pos);
}

void ChessManager::HandleCastling(Vector2 newPos, bool performed) {  //Si s'ha fet un enroque hem de recolocar les fitxes afectades
    if (!performed || newKingPos.row == -1)
        return;

    ChessPiece* kingToMove = whiteMoves ? whiteKing : blackKing;
    int row = whiteMoves ? 7 : 0;

    piecesAlive.erase(kingToMove->GetPos());
    board[kingToMove->GetPos().row][kingToMove->GetPos().column]->SetPiece(nullptr); //Actualitzem la posicio del rei
    board[newKingPos.row][newKingPos.column]->SetPiece(kingToMove);
    piecesAlive[newKingPos] = kingToMove;

    Vector2 newTowerPos = kingToMove->GetPos().column > 4 ? Vector2(row, 5) : Vector2(row, 3); //Actualitzem la posicio de la torre segons sigui esquerra o dreta
    int col = newTowerPos.column == 3 ? 0 : 7;
    ChessPiece* towerToMove = board[newTowerPos.row][col]->GetPiece();
    piecesAlive.erase(towerToMove->GetPos());
    board[towerToMove->GetPos().row][towerToMove->GetPos().column]->SetPiece(nullptr);
    board[newTowerPos.row][newTowerPos.column]->SetPiece(towerToMove);
    piecesAlive[newTowerPos] = towerToMove;
}

void ChessManager::HandlePromotion(SpecialMove& sMove, Vector2 newPos) {
    ChessPiece* p = selectedSquare->GetPiece();

    if (p == nullptr || p->GetType() != PieceType::PawnType || !(newPos.row == 0 || newPos.row == ChessSideLength - 1))
        return;

    sf::Text sfInputText;
    sfInputText.setFont(font); // Establecer la fuente
    sfInputText.setCharacterSize(16); // Establecer el tamaño del texto
    sfInputText.setPosition(0, GAMEHEIGTH + (TOTALHEIGTH - GAMEHEIGTH) / 3);
    sfInputText.setFillColor(sf::Color(70, 70, 70));
    sfInputText.setOrigin(0, -sfInputText.getGlobalBounds().height);

    sf::RectangleShape updateTextRect;
    updateTextRect.setFillColor(sf::Color(200, 200, 180));
    updateTextRect.setPosition(0, GAMEHEIGTH);
    updateTextRect.setSize( sf::Vector2f(GAMEWIDTH, TOTALHEIGTH - GAMEHEIGTH));

    std::string inputText;
    std::string defaultText = "Promotion available: 1 -> Knight | 2 -> Bishop | 3 -> Rook | 4 -> Queen: ";

    sfInputText.setString(defaultText);
    gameWindow->draw(sfInputText);
    gameWindow->display();
    bool choiceDone = false;

    while (!choiceDone) { //Si sha fet una promotion no sortir del bucle fins que es trii quina ficha es vol canviar
        sf::Event event;
        while (gameWindow->pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && inputText.size() == 1) { // Tecla "enter"
                choiceDone = inputText == "1" || inputText == "2" || inputText == "3" || inputText == "4";
            }
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128 && event.text.unicode != 8 && event.text.unicode != 13 && inputText.size() < 37) { // Caracteres ASCII sense "backspace" i "enter"
                    inputText += static_cast<char>(event.text.unicode);
                }
                else if (event.text.unicode == 8 && !inputText.empty()) { // Tecla "backspace"
                    inputText.pop_back();
                }

                sfInputText.setString(defaultText + inputText);
                gameWindow->draw(updateTextRect);
                gameWindow->draw(sfInputText);
                gameWindow->display();
            }
            
        }
    }        
    
    switch (static_cast<PieceType>(std::stoi(inputText)))
    {
    case PieceType::KnightType:
        selectedSquare->SetPiece(new Knight(whiteMoves, spriteSheet, PieceType::KnightType));
        break;
    case PieceType::BishopType:
        selectedSquare->SetPiece(new Bishop(whiteMoves, spriteSheet, PieceType::BishopType));
        break;
    case PieceType::RookType:
        selectedSquare->SetPiece(new Rook(whiteMoves, spriteSheet, PieceType::RookType));
        break;
    case PieceType::QueenType:
        selectedSquare->SetPiece(new Queen(whiteMoves, spriteSheet, PieceType::QueenType));
        break;
    default:
        break;
    }
    Packet newP;
    PromotionData data (newPos, selectedSquare->GetPiece()->GetType(), newP);
    serverSocket->Send(Promotion, newP);

    sMove = SpecialMove::PROMOTION;
}

Vector2 ChessManager::CanPerformCastling(ChessPiece* piece, ChessPiece* kingTarget, Vector2 pos) {
    ChessPiece* pieceInSpot = board[pos.row][pos.column]->GetPiece();
    if(piece == nullptr || pieceInSpot == nullptr)
        return Vector2(-1, -1);

    if (piece->GetFirstMoveDone() || pieceInSpot->GetFirstMoveDone() || !pathsToCheckingKing.empty()) //si alguna de las 2 peces ya sha mogut no es pot fer
        return Vector2(-1, -1);

    if (piece->GetType() == PieceType::KingType) {          //Si les peces triades no son rei o torre o viceversda tampoc es pot fer
        if (pieceInSpot->GetType() != PieceType::RookType)
            return Vector2(-1, -1);
    }
    else if (piece->GetType() == PieceType::RookType) {
        if (pieceInSpot->GetType() != PieceType::KingType)
            return Vector2(-1, -1);
    }
    bool pieceIsKing = piece == kingTarget;

    std::set<Vector2> moves = pieceIsKing ? pieceInSpot->GetCorrectMoves(piecesAlive) : piece->GetCorrectMoves(piecesAlive); //guardem les caselles per on es pot moure la torre
                                                                                                                             
    bool isLeftRook = pieceIsKing ? kingTarget->GetPos().column > pieceInSpot->GetPos().column : kingTarget->GetPos().column > piece->GetPos().column; //Mirem quina torre fem servir
    bool pathIsFree = false;
    int row = piece->GetIsWhite() ? 7 : 0;
    if(isLeftRook) //Les posicions a revisar per saber si el cami esta lliure dependran de quina torre hem agafat
        pathIsFree = moves.find(Vector2(row, 1)) != moves.end() && moves.find(Vector2(row, 2)) != moves.end() && moves.find(Vector2(row, 3)) != moves.end();
    else
        pathIsFree = moves.find(Vector2(row, 6)) != moves.end() && moves.find(Vector2(row, 5)) != moves.end();

    if (!pathIsFree)
        return Vector2(-1, -1);
     
    std::set<Vector2>castlingKingPath;
    castlingKingPath.insert(isLeftRook ? std::initializer_list<Vector2>{Vector2(row, 2), Vector2(row, 3)} : std::initializer_list<Vector2>{ Vector2(row, 6), Vector2(row, 5) });

    for (std::set<Vector2>::iterator it = castlingKingPath.begin(); it != castlingKingPath.end(); it++) {
        for (auto it2 = piecesAlive.begin(); it2 != piecesAlive.end(); it2++) {
            if (it2->second->GetIsWhite() == piece->GetIsWhite()) //Nomes ens interesa mirar el proxim jugador que ha de jugar
                continue;

            std::set<Vector2> positions = it2->second->GetPathToKillTarget(*it, piecesAlive); //Si alguna de les posicions on pasara el rey faria jaque, no el podem fer
            if (!positions.empty()) 
                return Vector2(-1, -1);            
        }
    }

    return isLeftRook ? Vector2(row, 2) : Vector2(row, 6); //Nova posicio del rey
}


SpecialMove ChessManager::CanDoSpecialMove(ChessPiece* piece, Vector2 selectedPos, bool* targetIsCorrect) {

    SpecialMove result = SpecialMove::NONE;
    Vector2 killPos;
    
    switch (piece->GetType())
    {
    case PieceType::PawnType:
        killPos = CanKillEnPassant(selectedSquare->GetPiece());
        if ((killPos.row >= 0) && (*targetIsCorrect |= (killPos == selectedPos))) { //Si es podia fer enpassant i l'hem triat ho maquem
            result = SpecialMove::ENPASSANT;
            Packet p;
            EnPassantData data(killPos, enPassantPawn->GetPos(), p);
            serverSocket->Send(EnPassant, p);
        }
        break;
    case PieceType::RookType:
    case PieceType::KingType:
        ChessPiece* kingTarget = whiteMoves ? whiteKing : blackKing;
        bool isRook = piece->GetType() == PieceType::RookType;
        int row = piece->GetIsWhite() ? 7 : 0;
        newKingPos = CanPerformCastling(piece, kingTarget, selectedPos);
        if ((newKingPos.row >= 0) && (*targetIsCorrect |= (isRook ? selectedPos == Vector2(row, 4) : (selectedPos == Vector2(row, 0) || selectedPos == Vector2(row, 7))))) { //Si es podia fer enroque i ho hem triat ho marquem
            result = SpecialMove::CASTLING;
            Packet p;
            Vector2 oldRookPos = Vector2(newKingPos.row, newKingPos.column < 4 ? 0 : 7);
            Vector2 newRookPos = Vector2(newKingPos.row, newKingPos.column < 4 ? 3 : 6);            
            CastleData data(newKingPos, newRookPos, oldRookPos, p);
            serverSocket->Send(Castle, p);
        }
        break;    
    }

    return result;
}

void ChessManager::CheckKingsChecks() 
{    
    pathsToCheckingKing.clear();        
    bool canBeCheckMate = false;
    King* turnTargetKing = whiteMoves ? whiteKing : blackKing;
    std::vector<std::set<Vector2>> turnTargetKingCheckPaths;

    std::vector<ChessPiece*>aliadePieces;

    for (auto it = piecesAlive.begin(); it != piecesAlive.end(); it++) { //Recorrem totes les fiches enemigues per veure si alguna esta fent jaque
        ChessPiece* kingTarget = it->second->GetIsWhite() ? blackKing : whiteKing; 
        if (kingTarget == nullptr) //Si el rei esta mort la partida ya ha acabat 
            return;
        if (it->second->GetIsWhite() == whiteMoves) //Nomes ens interesa mirar el proxim jugador que ha de jugar
            continue;

        aliadePieces.push_back(it->second);

        std::set<Vector2> positions = it->second->GetPathToKillTarget(kingTarget->GetPos(), piecesAlive); //Obtenim totes les posicions del cami que porta a un jaque (si existeix)
        if (!positions.empty()) {            
                
            pathsToCheckingKing.push_back(std::make_pair(kingTarget->GetIsWhite(), positions));    
            if (kingTarget == turnTargetKing) {
                positions.insert(it->second->GetPos());
                turnTargetKingCheckPaths.push_back(positions);
                turnTargetKingCheckPaths[turnTargetKingCheckPaths.size() - 1].erase(kingTarget->GetPos()); //Ens guardem totes les posicions q porten a un jaque per quan les haguem de pintar lila
                canBeCheckMate = true;
            }
        }
    }

    //Si el rei esta en jaque
    if (!canBeCheckMate)
        return;
        
    //Comprovar si te posicions segures disponibles 
    bool safePosAvailable = false;
    
    std::set<Vector2> kingMoves = turnTargetKing->GetCorrectMoves(piecesAlive);
    for (auto it = kingMoves.begin(); it != kingMoves.end(); it++) {
        safePosAvailable = !CheckIfKingVulnerable(*it, turnTargetKing, false);
        if (safePosAvailable) //Si hiha posicio segura no es jaque mate
        {
            return;
        }
    }      

    //Si no hiha posicio segura 
    if (turnTargetKingCheckPaths.size() <= 1) { //Si el rey solo tiene una ficha haciendole jaque i otra ficha puede eliminar al atacante o interponerse en el camino del jaque, no es jaque mate
        for (auto it = aliadePieces.begin(); it != aliadePieces.end(); it++) { //Per cada peça aliada
            if (*it == turnTargetKing)
                continue;
            std::set<Vector2> aliadeMoves = (*it)->GetCorrectMoves(piecesAlive); //Obtenem els seus moviments
            for (auto it2 = aliadeMoves.begin(); it2 != aliadeMoves.end(); it2++) {
                for (auto it3 = turnTargetKingCheckPaths.begin(); it3 != turnTargetKingCheckPaths.end(); it3++) {
                    auto it4 = std::find(it3->begin(), it3->end(), *it2); //Si algun moviment dun aliat pot bloquejar el jaque o matar qui el fa no es jaque mate
                    if (it4 != it3->end()) {
                        return;
                    }
                }
            }
            
        }
        
    }
    //Si el rei te mes duna ficha fent jaque o no sha pogut bloquejar/matar el seu unic jaque, es jaque mate    
    CheckMate(turnTargetKing);    
}

void ChessManager::LoadPiece(Vector2 keyPos, ChessPiece* piece)
{
    piecesAlive.insert(std::make_pair(keyPos, piece));
    board[keyPos.row][keyPos.column]->SetPiece(piece, true);
}

void ChessManager::GenerateColorFromSeed(sf::Uint64 seed, bool isFirst)
{
    // Crear un generador de números aleatorios con la semilla proporcionada
    std::mt19937 rng(seed);

    // Crear una distribución uniforme entre 0 y 99
    std::uniform_int_distribution<int> dist(0, 99);

    if(isFirst)
        clientIsWhite = dist(rng) % 2 == 0;
    else
        clientIsWhite = !(dist(rng) % 2 == 0);
}

void ChessManager::StartP2P()
{
    gameWindow->setTitle("CHESS - MAY THE BEST WIN");

    serverSocket->disconnect();
    for (int i = 0; i < p2pAdresses.size(); i++) {
        
        if (i == p2pAdresses.size() - 1) {
           
            std::cout << std::endl << "StartListener" << std::endl;
            StartListener(3001, sf::IpAddress::getLocalAddress().toString()); //StartListener(p2pAdresses[i]->port, p2pAdresses[i]->ip);
        }
        else {
            //std::this_thread::sleep_for(std::chrono::seconds(500));
            std::cout << std::endl << "Trying conect to " << p2pAdresses[i]->ip << std::endl;
            sf::Time t = sf::microseconds(500);
            if (sM->ConnectToServer(p2pAdresses[i]->ip, 3001, t)) {
               
                std::cout << "Conectado correctamente a la ip " + p2pAdresses[i]->ip << " puerto " << p2pAdresses[i]->port;
            }
            else {
                std::cout << "ERROR al conectarse correctamente a la ip " + p2pAdresses[i]->ip << " puerto " << p2pAdresses[i]->port;
            }

        }
    }

    p2pDone = true;
}

void ChessManager::StartListener(unsigned short port, std::string ip)
{    
    if (sM->StartListener(port, sf::IpAddress(ip))) //Intentarem començar a escoltar amb el listener del servidor
    {
        std::cout << "Listening on Ip: " << ip <<  " port " << std::to_string(port) << std::endl;
        sM->StartLoop(); //Si ho aconseguim començarem el bucle d'execucio 
        return;
    }
    std::cout << "Error on turning server up on port " << std::to_string(port) << " and ip " << ip << std::endl;
    return;    
}

void ChessManager::CheckIfReady(sf::Vector2i mousePos)
{
    sf::Event event;
    while (gameWindow->pollEvent(event))
    {
        if (event.type == sf::Event::Closed) { //Si donem a la creu de la finestra aquesta es tanca
            gameWindow->close();
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

            if (*currentClientState == ClientState::PLAYER && showReadyButton && readyButton->getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                showReadyButton = false;
                Packet p;
                p << bool(true);
                serverSocket->Send(SetReady, p);
            }
        }
        else {
            chatManager->HandleInput(event);
        }
    }
}

bool ChessManager::CheckIfKingVulnerable(Vector2 newPos, ChessPiece* selectedPiece, bool printWarningText) //Comprova si el moviment triat deixaria al nostre rei vulnerable
{
    std::map<Vector2, ChessPiece*> piecesInBoardAfterMove = piecesAlive;
    piecesInBoardAfterMove[newPos] = selectedPiece;
    piecesInBoardAfterMove.erase(selectedPiece->GetPos());

    for (auto it = piecesInBoardAfterMove.begin(); it != piecesInBoardAfterMove.end(); it++) {
        ChessPiece* kingTarget = it->second->GetIsWhite() ? blackKing : whiteKing;
        if (kingTarget == nullptr || kingTarget->GetIsWhite() != selectedPiece->GetIsWhite()) //Si volem comprovar si es queda indefens, nomes volem comprovar el nostre rei, no el contrari
            continue;
        std::set<Vector2> positions = it->second->GetPathToKillTarget(kingTarget == selectedPiece ? newPos : kingTarget->GetPos(), piecesInBoardAfterMove);
        if (!positions.empty()) {
            if (printWarningText) {
                errorText.setString("INVALID: KING WOULD BE VULNERABLE");
                errorText.setOrigin(errorText.getGlobalBounds().width * 0.5f, -errorText.getGlobalBounds().height);
            }
            selectedSquare = nullptr;
            return true;            
        }
    }
    return false;
}

void ChessManager::HandleEnPassant(Vector2 newPos, bool performed)
{
    if (performed) {
        board[enPassantPawn->GetPos().row][enPassantPawn->GetPos().column]->SetPiece(nullptr); //Si s'ha completat matar el peo afectat
    }
    if (enPassantPawn != nullptr && enPassantPawn->GetIsWhite() == whiteMoves) //Si havia opcio de enpassant i torna a tocar a aquell juagdor es desactiva la opcio
        enPassantPawn = nullptr;

    if (Pawn* pawn = dynamic_cast<Pawn*>(selectedSquare->GetPiece())) { //Si un peo acaba de fer el doble moviment s'activa com a enPassant actual
        if (std::abs(pawn->GetPos().row - newPos.row) == 2) {
            enPassantPawn = pawn;
        }
    }
}

void ChessManager::CheckMate(King* selectedKing)
{
    std::string winString = "CHECKMATE: " + (std::string)(whiteMoves ? "BLACK" : "WHITE") + " WINS";
    errorText.setPosition(GAMEWIDTH * 0.5f - 100, TOTALHEIGTH - 35);
    errorText.setString(winString);      
    errorText.setFillColor(sf::Color::Magenta);
    currentGS = whiteMoves ? GAMESTATE::BLACKWINS : GAMESTATE::WHITEWINS;    
}
