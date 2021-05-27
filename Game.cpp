#include <fstream>
#include <iostream>

#include "Game.h"

#define ASCII_VALUE_A 65

Game::Game(int playerCount) {
    this->board = new Board;
    this->tileBag = new LinkedList();
    this->pCount = 0;
    this->players = new Player*[playerCount]{};
    this->currentPlayer = -1;
    this->firstTurn = true;

    // Filling the bag with 72 tiles, coloursArray should be GC'd after
    char colorsArray[NUM_COLOURS] {RED,ORANGE,YELLOW,GREEN,BLUE,PURPLE};
    for (int colour = 0; colour < NUM_COLOURS; colour++)
    {
        // Forgot the offset as shapes go 1-6 not 0-5
        for (int shape = 1; shape < NUM_SHAPES+1; shape++)
        {
            Tile* tile = new Tile(colorsArray[colour], shape);
            // Will just call the add method twice instead of putting in a loop
            tileBag->addFront(tile);
            tileBag->addFront(tile);
            delete tile;
        }
    }
    
    // shuffle the contents around
    srand(time(NULL));
    for (size_t i = 0; i < 256; i++) {
        int rando = rand() % 72;
        tileBag->addFront(tileBag->get(rando));
        tileBag->remove(rando+1);
    }
}

Game::Game(Game& other) {
    this->board = new Board(*other.board);
    this->tileBag = new LinkedList(*other.tileBag);
    this->players = new Player*[other.pCount]{};
    for (int i = 0; i <other.pCount ; ++i) {
        this->addPlayer(other.getPlayer(i));
    }
    currentPlayer = other.currentPlayer;
}

Game::~Game() {
    delete board;
    delete tileBag;
    for (int i = 0; i < this->pCount ; ++i) {
        delete getPlayer(i);
    }
    delete[] players;
}

void Game::dealPlayerTiles()
{
    for (int x = 0; x < getPlayerCount(); ++x) {
        // Don't forget to get rid of this magic number later
        for (size_t i = 0; i < 6; i++) {
            getPlayer(x)->addToHand(tileBag->getFront());
            tileBag->removeFront();
        }
    }
}

void Game::printGame() {
    // Debug prints, remove later.
    std::cout << tileBag->getSize() << std::endl;
    std::cout << tileBag->toString() << std::endl;

    //std::cout << "p1 hand: " << player1->getHand() << std::endl;
    //std::cout << "p2 hand: " << player2->getHand() << std::endl;
}

void Game::addPlayer(Player* newPlayer) {
    Player* newP = new Player(*newPlayer);
    this->players[this->getPlayerCount()] = newP;
    this->pCount++;
}

int Game::getPlayerCount() {
    return this->pCount;
}

Player* Game::getPlayer(int i) {
    return players[i];
}

Player* Game::getCurrentPlayer() {
    return getPlayer(currentPlayer);
}

void Game::setCurrentPlayer(int currentPlayerNo) {
    currentPlayer = currentPlayerNo;
}

void Game::nextPlayer(){
    setCurrentPlayer((currentPlayer+1) % pCount);
}

Board* Game::getBoard() {
    return board;
}

LinkedList* Game::getTileBag() {
    return tileBag;
}

void Game::setBoard(Board& b) {
    if (board != nullptr) {
        delete board;
    }

    board = new Board(b);
}

void Game::setTileBag(LinkedList& tb) {
    if (tileBag != nullptr) {
        delete tileBag;
    }

    tileBag = new LinkedList(tb);
}

bool Game::placeTile(Tile& tile, char row, int col) {
    bool inputValid = false;
    bool nCheck = false;
    bool eCheck = false;
    bool sCheck = false;
    bool wCheck = false;
    bool vCheck = false;
    bool invCheck = false;
    bool hCheck = false;
    bool inhCheck = false;
    int rowIndex = rowCharToIndex(row);

    // Check if theres already a tile in desired location
    Tile* thisPos = board->tileAt(row,col);
    if (thisPos == nullptr) {
        // Check if there's a tile to the north
        Tile* nPos = board->tileAt(rowIndex - 1, col);
        if (nPos != nullptr) {
            nCheck = true;
        }
        delete nPos;

        // Check if there's a tile to the east
        Tile* ePos = board->tileAt(rowIndex, col + 1);
        if (ePos != nullptr) {
            eCheck = true;
        }
        delete ePos;

        // Check if there's a tile to the south
        Tile* sPos = board->tileAt(rowIndex + 1, col);
        if (sPos != nullptr) {
            sCheck = true;
        }
        delete sPos;

        // Check if there's a tile to the west
        Tile* wPos = board->tileAt(rowIndex, col - 1);
        if (wPos != nullptr) {
            wCheck = true;
        }
        delete wPos;

        // Check if there are no neighbours
        if (nCheck || eCheck || sCheck || wCheck) {
            // Check if tile fits within entire vertical line
            if (nCheck || sCheck) {
                vCheck = validateTilesInDirection(tile,rowIndex,col,1,0);
                invCheck = validateTilesInDirection(tile,rowIndex,col,-1,0);
            }
                
            // Check if tile fits within entire horizontal line
            if (eCheck || wCheck) {
                hCheck = validateTilesInDirection(tile,rowIndex,col,0,1);
                inhCheck = validateTilesInDirection(tile,rowIndex,col,0,-1);
            }
        }
    }
    delete thisPos;

    // If vertical line was attempted and failed, or horizontal line was
    // attempted and failed skip marking validation as true
    if (((nCheck || sCheck) && (vCheck && invCheck)) ||
        ((eCheck || wCheck) && (hCheck && inhCheck))) { 
        inputValid = true;
    }

    if (((nCheck || sCheck) && !(vCheck && invCheck)) || 
        ((eCheck || wCheck) && !(hCheck && inhCheck))) {
        inputValid = false;
    } 

    if (inputValid || firstTurn) {
        inputValid = board->placeTile(tile, row, col);
        // Adds the score of the move, ternary is for very first move to be (+1)
        getCurrentPlayer()->setScore(getCurrentPlayer()->getScore() 
            + ((firstTurn)? 1: scoreTile(tile, rowIndex, col)));
        firstTurn = false;
    }

    return inputValid;
}

bool Game::validateTilesInDirection(Tile& tile, int originX, int originY,
                                    int moveX, int moveY) {
    //set result to true
    bool result = true;
    //set similarAttribute to null
    string similarAttributeType = "";
    int multiplier = 1;
    Tile* nextTile = board->tileAt(originX + moveX * multiplier,
                                   originY + moveY * multiplier);
    
    while (result && nextTile != nullptr) {
        // if similarAttribute condition is null, set one here
        if (similarAttributeType == "") {
            // If tiles share colour, set similar attribute to colour
            if (nextTile->getColour() == tile.getColour()) {
                similarAttributeType = "COLOUR";
            }

            // If tile is sharing shape and colour, its a duplicate and should
            // be rejected, otherwise if its only matching shape, set similar
            // attribute to shape
            if (nextTile->getShape() == tile.getShape()) {
                similarAttributeType =
                    (similarAttributeType != "") ? "" : "SHAPE";
            }
        }

        // If similarAttribute is still empty, tile does not match required
        // conditions
        if (similarAttributeType == "") {
            result = false;
        } else {
            // If tiles share the same colour, check that shape is different
            if (similarAttributeType == "COLOUR") {
                result = (nextTile->getShape() != tile.getShape() &&
                          nextTile->getColour() == tile.getColour());
            }
                
            // If tiles share the same shape, check that colour is different
            if (similarAttributeType == "SHAPE") {
                result = (nextTile->getColour() != tile.getColour() && 
                          nextTile->getShape() == tile.getShape());
            }   

            // Move to next tile
            multiplier += 1;
            delete nextTile;
            nextTile = board->tileAt(originX + moveX * multiplier,
                                     originY + moveY * multiplier);
        }
    }

    // Check if no tiles where found in set direction
    if (multiplier == 1 && nextTile == nullptr) {
        result = true;
    }
    delete nextTile;
    return result;
}

int Game::scoreTile(Tile& tile, int row, int col) {
    int score = 0;
    int multiplier = 1;
    Tile* nextTile = board->tileAt(row + multiplier, col);
    int tileCount = 1;

    // Check number of tiles in +ve row direction
    while (nextTile != nullptr) {
        tileCount += 1;
        multiplier += 1;
        delete nextTile;
        nextTile = board->tileAt(row + multiplier, col);
    }

    // Check number of tiles in -ve row direction
    multiplier = 1;
    nextTile = board->tileAt(row - multiplier, col);
    while (nextTile != nullptr) {
        tileCount += 1;
        multiplier += 1;
        delete nextTile;
        nextTile = board->tileAt(row - multiplier, col);
    }

    // If tile was found to be within a row score accordingly
    if (tileCount > 1) {
        score += tileCount;
        // Apply bonus score if quirkle
        if (tileCount >= NUM_COLOURS) {
            std::cout << "QWIRKLE!!!" << std::endl << std::endl;
            score += NUM_COLOURS;
        }
        
    }

    // Check number of tiles in +ve column direction
    tileCount = 1;
    multiplier = 1;
    nextTile = board->tileAt(row, col + multiplier);
    while (nextTile != nullptr) {
        tileCount += 1;
        multiplier += 1;
        delete nextTile;
        nextTile = board->tileAt(row, col + multiplier);
    }

    // Check number of tiles in -ve column direction
    multiplier = 1;
    nextTile = board->tileAt(row, col - multiplier);
    while (nextTile != nullptr) {
        tileCount += 1;
        multiplier += 1;
        delete nextTile;
        nextTile = board->tileAt(row, col - multiplier);
    }


    // If tile was found to be within a row score accordingly
    if (tileCount > 1) {
        score += tileCount;
        // Apply bonus score if quirkle
        if (tileCount >= NUM_COLOURS) {
            std::cout << "QWIRKLE!!!" << std::endl;
            score += NUM_COLOURS;
        }
    }
    delete nextTile;
    return score;
}

bool Game::saveGame(std::string filename) {
    bool isSaved = false;

    try {
        std::ofstream outFile;
        outFile.open(filename);

        //Write total player count
        outFile << pCount << std::endl;
        
        // Write players
        for (int i = 0; i < pCount; ++i) {
            outFile << players[i]->serialise();
        }

        // Write board shape
        outFile << std::to_string(board->getHeight()) << ","
                << std::to_string(board->getWidth()) << std::endl;

        // Write board state
        outFile << board->serialise() << std::endl;

        // Write tile bag
        outFile << tileBag->toString() << std::endl;

        // Write current player
        if (currentPlayer != -1) {
            outFile << getCurrentPlayer()->getName();
        }

        outFile << std::endl;
        outFile.close();

        isSaved = true;
    } catch (...) {
        // Something went wrong, isSaved remains false
    }

    return isSaved;
}

int Game::rowCharToIndex(char row) {
    row = std::toupper(row);

    /*
     * ASCII uppercase characters begin from 65, so subtracting 65 from the
     * character's value produces the corresponding row index in the vector.
     * 
     * A useful table can be found here:
     * https://www.w3schools.com/charsets/ref_html_ascii.asp
     */ 
    return (int) row - ASCII_VALUE_A;
}

bool Game::removeTileCurrPlayer(Tile* t){
    return getCurrentPlayer()->removeFromHand(t);
}

bool Game::addTileToTileBag(Tile* t){
    tileBag->addBack(t);
    return true;
}

bool Game::swapTile(Tile* t){
    bool success = false;
    if (tileBag->getSize() > 0)
    {
        getCurrentPlayer()->removeFromHand(t);
        tileBag->addBack(t);
        Tile *sTile = new Tile(*tileBag->getFront());
        getCurrentPlayer()->addToHand(sTile);
        tileBag->removeFront();
        success = true;

        // Will this be needed? 
        delete sTile;
    }
    return success;
}

void Game::drawATile(){
    Tile *nTile = new Tile(*tileBag->getFront());
    getCurrentPlayer()->addToHand(nTile);
    tileBag->removeFront();
    delete nTile;
}

Player* Game::getWinner() {
    int score = 0;
    Player* winner = nullptr;

    for (int i = 0; i < pCount; i++)
    {
        if (players[i]->getScore() > score)
        {
            winner = players[i];
        }
    }
    return winner;
}

void Game::skipFirstTurn() {
    firstTurn = false;
}