#ifndef ASSIGN2_GAME_H
#define ASSIGN2_GAME_H

#include "Board.h"
#include "LinkedList.h"
#include "Player.h"
#include "TileCodes.h"

class Game
{
private:
    Board* board;
    LinkedList* tileBag;
    Player** players;
    int pCount;
    bool firstTurn;

    int currentPlayer;
    int rowCharToIndex(char row);

public:
    Game(int playerCount);
    Game(Game& other);
    ~Game();

    bool placeTile(Tile& tile, char row, int col);
    bool validateTile(Tile& tile, int row, int col);

    bool validateTilesInDirection(Tile& tile, int originX, int originY,
                                  int moveX, int moveY);
    int scoreTile(Tile& tile, int row, int col,bool printQwirkles);

    // Attempts to save the game to a file with the given name. Returns true
    // if successful.
    //
    // Ends the file with a newline and overwrites the file if it exists.
    bool saveGame(std::string filename);


    int getPlayerCount();
    void dealPlayerTiles();
    void addPlayer(Player* newPlayer);
    void printGame();
    Player* getPlayer(int i);
    Player* getCurrentPlayer();
    void setCurrentPlayer(int currentPlayerNo);
    void nextPlayer();

    Board* getBoard();
    LinkedList* getTileBag();

    void setBoard(Board& b);
    void setTileBag(LinkedList& tb);
    bool removeTileCurrPlayer(Tile* t);
    bool addTileToTileBag(Tile* t);

    bool swapTile(Tile* t);
    void drawATile();
    Player* getWinner();
    void skipFirstTurn();
};

#endif // ASSIGN2_GAME_H