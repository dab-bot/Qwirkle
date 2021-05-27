#include "GameController.h"

#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include <limits>

using std::cin;
using std::cout;
using std::endl;
using std::istringstream;
using std::string;

GameController::GameController(int playerCount, bool colouredTiles)
{
    this->colouredTiles = colouredTiles;
    this->game = new Game(playerCount);
    this->pCount = playerCount;
    for (int i = 0; i < pCount; ++i)
    {
        addPlayer();
    }
    this->keepGoing = true;
}

GameController::GameController(Player* players[],unsigned int playerCount, Board &board,
                               LinkedList &tileBag, int currentPlayerNo,
                               bool firstTurn, bool colouredTiles)
{
    // For milestone 2, this constructor only creates 2-player games.
    this->colouredTiles = colouredTiles;
    this->pCount = playerCount;
    this->game = new Game(pCount);
    for(unsigned int i = 0; i < playerCount; i++){
        game->addPlayer(players[i]);
    }
    game->setCurrentPlayer(currentPlayerNo);

    game->setBoard(board);
    game->setTileBag(tileBag);

    this->keepGoing = true;

    if (!firstTurn)
    {
        game->skipFirstTurn();
    }

    // If this is not included, the game status will not be displayed on the
    // first turn of a loaded game.
    printScoreBoardHand();
}

GameController::~GameController()
{
    if (game != nullptr)
    {
        delete game;
    }
}

void GameController::addPlayer()
{
    bool correct = false;
    string input = "";
    while (correct == false)
    {
        cout << "Enter a name for player " << game->getPlayerCount() + 1
             << " (uppercase characters only)" << endl;
        cout << "> ";
        std::getline(std::cin, input);

        if (validate_PlayerName(input))
        {
            correct = true;
        }
        else
        {
            cout << "Invalid Input: name is not exclusively UPPERCASE" << endl;
        }
    }
    Player *newP = new Player(input);
    game->addPlayer(newP);
    delete newP;
}

void GameController::gameStart()
{
    game->dealPlayerTiles();
    game->setCurrentPlayer(0);
    printScoreBoardHand();
}

void GameController::gameLoop()
{
    while (keepGoing)
    {
        bool moveSuccess = false;

        // Ask current player for thier move
        string input = askForPlayerMove();
        if (keepGoing)
        {
            // Validate and execute move
            moveSuccess = validateAndExecute(input);

            // If the last move emptied the player hand, end the game, this can
            // only happen after the TileBag was emptied, so no need to check
            // for that.
            if (game->getCurrentPlayer()->getHand()->getSize() == 0)
            {
                cout << game->getBoard()->toString() << endl;
                game->getCurrentPlayer()->setScore(
                    (game->getCurrentPlayer()->getScore() + 6));
                keepGoing = false;
                cout << "Game over" << endl;
                for (int i = 0; i < pCount; ++i)
                {
                    cout << "Score for " << game->getPlayer(i)->getName() << ": "
                         << game->getPlayer(i)->getScore() << endl;
                }
                cout << "Player " << game->getWinner()->getName() << " won!"
                     << endl;
            }
            else
            {
                // switch current player if move was a success, and reprint
                // board, but not if the game has finished
                if (moveSuccess == true)
                {
                    game->nextPlayer();
                    printScoreBoardHand();
                }
            }
        }
    }
}

void GameController::skipFirstTurn()
{
    game->skipFirstTurn();
}

string GameController::askForPlayerMove()
{
    string input = "";

    cout << endl;
    cout << "> ";
    std::getline(std::cin, input);
    cout << endl;
    if (std::cin.eof())
    {
        keepGoing = false;
    }
    return input;
}

bool GameController::validateAndExecute(string input)
{
    bool moveSuccess = false;

    // This block borrowed and modified from:
    // https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
    std::istringstream iss(input);
    std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                     std::istream_iterator<std::string>());
    // End derived block

    if (validate_Place(input))
    {
        // Valid place command given
        moveSuccess = makeAMove(results.at(1), results.at(3));
    }
    else if (validate_Replace(input))
    {
        // Valid replace command given
        moveSuccess = replaceATile(results.at(1));
    }
    else if (validate_save(results))
    {
        // Because validate_save() is true, we know there is a second token
        std::string filename = results.at(1);

        if (game->saveGame(filename))
        {
            std::cout << "Game successfully saved" << std::endl;
        }
        else
        {
            std::cout << "Sorry, there was an error while saving the game"
                      << std::endl;
        }
    }
    else if (results.size() > 0 && results.at(0) == "quit")
    {
        // Valid quit command given
        keepGoing = false;
    }

    else
    {
        // Invalid command given
        std::cout << "Invalid Input" << std::endl;
    }

    return moveSuccess;
}

bool GameController::makeAMove(string tileSTR, string moveSTR)
{
    bool success = false;
    Tile *mTile = new Tile(tileSTR);

    string colSTR = moveSTR.substr(1, 2);
    int col = std::stoi(colSTR);

    // Check if the player hand has the tile or not
    if (game->getCurrentPlayer()->hasTile(mTile))
    {
        // Try to place the tile if the player has it
        success = game->placeTile(*mTile, moveSTR.at(0), col);

        // Remove tile from player hand if the move was valid
        if (success)
        {
            game->removeTileCurrPlayer(mTile);
            // Draw new tile from tileBag if there are any left
            if (game->getTileBag()->getSize() > 0)
            {
                game->drawATile();
            }
        }
        else
        {
            std::cout << "Move is invalid." << std::endl;
        }
    }
    else
    {
        std::cout << "You don't have that tile" << std::endl;
    }
    delete mTile;
    return success;
}

bool GameController::replaceATile(string tileSTR)
{
    Tile *rTile = new Tile(tileSTR);
    bool success = false;

    // Check if the player has that tile or not, if yes...
    if (game->getCurrentPlayer()->hasTile(rTile))
    {
        success = game->swapTile(rTile);
    }
    // If the action didn't succeed, we should tell the player
    if (success == false)
    {
        std::cout << "Replacement not successful." << std::endl;
        if (game->getTileBag()->getSize() < 1)
        {
            std::cout << "Tile bag is empty!" << std::endl;
        }
    }
    delete rTile;
    return success;
}

bool GameController::validate_Place(string input)
{
    // https://www.softwaretestinghelp.com/regex-in-cpp/
    // Modified from here
    // regex expression for pattern to be searched
    std::regex regex("^place [ROYGBP][1-6] at [A-Z][0-9]{1,2}$");
    // flag type for determining the matching behavior (in this case on string
    // objects)
    std::smatch m;
    // regex_search that searches pattern in the string
    return std::regex_search(input, m, regex);
}

bool GameController::validate_Replace(string input)
{
    // https://www.softwaretestinghelp.com/regex-in-cpp/
    // Modified from here
    std::regex regex("^replace [ROYGBP][1-6]$");
    std::smatch m;
    return std::regex_search(input, m, regex);
}

bool GameController::validate_PlayerName(string input)
{
    // https://www.softwaretestinghelp.com/regex-in-cpp/
    // Modified from here
    std::regex regex("^[A-Z]+$");
    std::smatch m;
    return std::regex_search(input, m, regex);
}

bool GameController::validate_save(std::vector<std::string> &input)
{
    bool isValid = false;

    if (input.size() > 0)
    {
        isValid = input.size() == 2 && input.at(0) == "save";
    }

    return isValid;
}

void GameController::printScoreBoardHand()
{
    // Print current state of the game/board
    cout << endl
         << game->getCurrentPlayer()->getName() << ", it's your turn"
         << endl;
    for (int i = 0; i < pCount; ++i)
    {
        cout << "Score for " << game->getPlayer(i)->getName() << ": "
                << game->getPlayer(i)->getScore() << endl;
    }

    if (colouredTiles)
    {
        cout << game->getBoard()->toConsoleString() << endl
             << endl
             << "Your hand is " << endl
             << game->getCurrentPlayer()->getHand()->toConsoleString() << endl;
    }
    else
    {
        cout << game->getBoard()->toString() << endl
             << endl
             << "Your hand is " << endl
             << game->getCurrentPlayer()->getHand()->toString() << endl;
    }
}