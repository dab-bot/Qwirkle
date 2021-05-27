#ifndef ASSIGN2_GAMECONTROLLER_H
#define ASSIGN2_GAMECONTROLLER_H

#include "Game.h"

class GameController
{
private:
    Game* game;
    int pCount;
    bool keepGoing;
    bool colouredTiles;
public:
    GameController(int playerCount, bool colouredTiles);
    GameController(Player* players[],unsigned int playerCount, Board& board, LinkedList& tileBag,
                   int currentPlayerNo, bool firstTurn, bool colouredTiles);
    ~GameController();

    void addPlayer();
    void gameStart();
    void gameLoop();
    void skipFirstTurn();

    string askForPlayerMove();
    bool validateAndExecute(string input);

    bool makeAMove(string tileSTR, string moveSTR);
    bool replaceATile(string tileSTR);

    bool validate_Place(string input);
    bool validate_Replace(string input);
    bool validate_PlayerName(string input);
    bool validate_save(std::vector<std::string>& input);
    void printScoreBoardHand();

};

#endif // ASSIGN2_NODE_H