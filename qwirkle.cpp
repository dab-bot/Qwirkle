#include "Game.h"
#include "GameController.h"
#include "LinkedList.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <vector>

#define EXIT_SUCCESS 0

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::istringstream;

string promptUser();
void startNewGame();
bool loadGame();
void showCredits();
void terminateGame();

bool isColoured = true;
bool againstAI = false;

// Split inputString into tokens based on the locations of delimiter and return
// them as a vector
std::vector<string> splitString(string inputString, string delimiter);

// Validate that a colour and shape are valid Tile colours and shapes
bool validateTile(char colour, int shape);

// Returns user input as std::string which can be parsed as int, where required,
// using string stream (as demonstrated in menu)
string promptUser() {
    string input = "";
    cout << "> ";
    // cin >> input;
    std::getline(std::cin, input);
    return input;
}

void startNewGame() {
    cout << "Starting a New Game" << endl << endl;
    cout << "How many Players would you like?" << endl << endl;
    int numPlayers = 0;
    while (numPlayers < 2 || numPlayers > 4){
        try{
            numPlayers = std::stoi(promptUser());
            if (numPlayers < 2 || numPlayers > 4)
                cout << "Please input a number from 2-4" << endl << endl;
        }catch(...){
            cout << "Please input a valid number" << endl << endl;
        }
    }
    GameController* theGame = new GameController(numPlayers,isColoured);
    cout << "Let's Play!" << endl;
    theGame->gameStart();
    theGame->gameLoop();
    delete theGame;
}

void startAIGame() {
    againstAI = true;
    cout << "Starting a New Game" << endl << endl;
    GameController* theGame = new GameController(isColoured,againstAI);
    cout << "Let's Play!" << endl;
    theGame->gameStart();
    theGame->gameLoop();
    delete theGame;
}

bool loadGame() {
    // Get the filename from the user
    cout << "Enter the filename from which to load a game" << endl;
    string filename = promptUser();

    bool success = false;
    std::vector<string> lines;
    bool newSaveFormat = true;

    // Attempt to read the file
    try {
        std::ifstream inFile;
        inFile.open(filename);

        // Check that the file exists and store the contents of the file
        if (inFile.good()) {
            // Store each line of the file in the vector
            while (!inFile.eof()) {
            string currLine;
            std::getline(inFile, currLine);
            lines.push_back(currLine);
            }
        }

        inFile.close();
    } catch (...) {
        // Error while reading file, success remains false
    }
    unsigned int pCount = 0;
    if(lines.at(0) == "#NewFormatSave"){
        lines.erase(lines.begin());
        newSaveFormat = true;
        //get player count from first line then remove it
        try {
            pCount = std::stoi(lines.at(0));
            lines.erase(lines.begin());
        } catch (...) {
        }
    }else{
        pCount = 2;
    }

    // Check that the format of the file is correct
    const int linesPerPlayer = (newSaveFormat)?4:3;
    const int gameStateLines = 4;

    // Verify the file had enough lines in it as a sanity check
    // AND verify theres enough players for a game
    if (lines.size() >= (linesPerPlayer * pCount) + gameStateLines && pCount>=2) {
        try {
            bool formatIsValid = true;

            Player* players[pCount];
            for (unsigned int i = 0; i < pCount; ++i) {
                players[i] = nullptr;
            }

            // Create players
            for (unsigned int i = 0; i < pCount; ++i) {
                string name = lines.at((i * linesPerPlayer));
                players[i] = new Player(name);
                std::vector<string> tileStrings;

                if(newSaveFormat){
                    //Set player's AI status
                    bool aiStatus = false;
                    if(lines.at((i * linesPerPlayer) + 1) == "AI")
                        aiStatus = true;
                    players[i]->setAIStatus(aiStatus);
                    // Set player's score
                    int score = std::stoi(lines.at((i * linesPerPlayer) + 2));
                    players[i]->setScore(score);

                    // Fill player's hand
                    tileStrings = splitString(lines.at((i * linesPerPlayer) + 3), ",");
                }else{
                    // Set player's score
                    int score = std::stoi(lines.at((i * linesPerPlayer) + 1));
                    players[i]->setScore(score);

                    // Fill player's hand
                    tileStrings = splitString(lines.at((i * linesPerPlayer) + 2), ",");
                }

                
                for (unsigned int j = 0; j < tileStrings.size(); ++j) {
                    char colour = tileStrings.at(j).at(0);
                    int shape = std::stoi(tileStrings.at(j).substr(1, 1));

                    // If the colour/shape is invalid, the tile will still be
                    // added to the player's hand, however the format of the
                    // file will be considered invalid
                    formatIsValid = formatIsValid &&
                                    validateTile(colour, shape);

                    Tile* t = new Tile(colour, shape);
                    players[i]->addToHand(t);
                    delete t;
                }
            }

            // Create board
            Board board;
            bool firstTurn = false;

            // Add tiles to board
            int boardTilesLine = (linesPerPlayer * pCount) + 1;
            std::vector<string> placedTiles =
                splitString(lines.at(boardTilesLine), ", ");

            for (unsigned int i = 0; i < placedTiles.size(); ++i) {
                std::vector<string> tileAndPos = splitString(placedTiles.at(i),
                                                             "@");
                if (tileAndPos.size() != 2) {
                    // Format is invalid, unless the board is empty in which
                    // case there is only an empty string in the board-tile line
                    if (!(placedTiles.size() == 1 && placedTiles.at(0) == "")) {
                        formatIsValid = false;
                    } else {
                        // Board is empty, so the next move is the first one
                        firstTurn = true;
                    }
                } else {
                    char tileColour = tileAndPos.at(0).at(0);
                    int tileShape = std::stoi(tileAndPos.at(0).substr(1, 1));
                    Tile* t = new Tile(tileColour, tileShape);

                    // Mark file format as invalid if tile is invalid
                    formatIsValid = formatIsValid &&
                                    validateTile(tileColour, tileShape);

                    // Add tile to board
                    char tileRow = tileAndPos.at(1).at(0);
                    int tileCol = std::stoi(tileAndPos.at(1).substr(1));

                    board.placeTile(*t, tileRow, tileCol);
                    delete t;
                }
            }

            // Create tile bag
            std::vector<string> bagTiles =
                splitString(lines.at((linesPerPlayer * pCount) + 2), ",");

            LinkedList tileList;
            for (unsigned int i = 0; i < bagTiles.size(); ++i) {
                string tileString = bagTiles.at(i);

                if (tileString.size() != 2) {
                    // An empty tile bag is considered valid
                    if (!(bagTiles.size() == 1 && bagTiles.at(0) == "")) {
                        formatIsValid = false;
                    }
                } else {
                    char tileColour = tileString.at(0);
                    int tileShape = std::stoi(tileString.substr(1, 1));
                    Tile* t = new Tile(tileColour, tileShape);

                    // Mark file format as invalid if tile is invalid
                    formatIsValid = formatIsValid &&
                                    validateTile(tileColour, tileShape);

                    tileList.addBack(t);
                    delete t;
                }
            }

            // Store current player
            string currPlayerName =
                lines.at((linesPerPlayer * pCount) + 3);

            // Determine which player is the current player
            unsigned int currPlayerNo = 0;
            for (unsigned int i = 0; i < pCount; ++i) {
                Player* p = players[i];
                if (p != nullptr && p->getName() == currPlayerName) {
                    currPlayerNo = i;
                }
            }

            if (currPlayerNo < 0 || currPlayerNo >= pCount) {
                formatIsValid = false;
            }

            // Create gameController & game, then begin the game if the file
            // was a valid save
            if (formatIsValid) {
                success = true;

                cout << "Qwirkle game successfully loaded" << endl;
                GameController* theGame = new GameController(players,
                                                             pCount,
                                                             board,
                                                             tileList,
                                                             currPlayerNo,
                                                             firstTurn,
                                                             isColoured);
                theGame->gameLoop();
                delete theGame;
            } else {
                cout << "The file loaded is not a valid Qwirkle save." << endl;
            }

            // Clean up memory
            for (unsigned int i = 0; i < pCount; ++i) {
                if (players[i] != nullptr) {
                    delete players[i];
                }
            }

        } catch (...) {
            // Error occurred while constructing game, success remains false
            cout << "An error occurred while loading the game." << endl;
        }
    }

    return success;
}

void terminationMessage() {
    cout << "Goodbye" << endl;
}

void showCredits() {
    cout << "-------------------------------------" << endl;
    cout << "Name: Ahmad Seiam Farighi" << endl << "Student ID: s3842662"
         << endl << "Email: s3842662@student.rmit.edu.au" << endl << endl;

    cout << "Name: Daniel Marmion" << endl << "Student ID: s3842912" << endl
         << "Email: s3842912@student.rmit.edu.au" << endl << endl;

    cout << "Name: Richard Forsey" << endl << "Student ID: s3857811" << endl
         << "Email: s3857811@student.rmit.edu.au" << endl << endl;

    cout << "Name: Aaron Fisher" << endl << "Student ID: s3840619" << endl
         << "Email: s3840619@student.rmit.edu.au" << endl;
    cout << "-------------------------------------" << endl << endl;
}

void showOptions() {
bool shouldDisplayMenu = true;
    do {
        cout << "Options" << endl << "----" << endl;
        cout << "1. Allow coloured tiles: " + std::string(isColoured ? "TRUE" : "FALSE") << endl;
        cout << "2. Back" << endl << endl;
        
        istringstream iss (promptUser());
        int selection = 0;
        iss >> selection;

        cout << endl;

        if (iss.fail()) {
            if (iss.eof()) {
                shouldDisplayMenu = false;
            } else {
                cout << "Invalid Input. Please enter a number from 1-2."
                     << endl << endl;
            }
        } else {
            if (selection == 1) {
                isColoured = (isColoured)?false:true;
            } else if (selection == 2) {
                shouldDisplayMenu = false;
            } else {
                cout << "Sorry, that isn't an option. "
                     << "Please enter a number from 1-2." << endl;
            }
        }
    } while (shouldDisplayMenu);
}

int main(void) {
    cout << "Welcome to Quirkle!" << endl << "-------------------" << endl;
    atexit(terminationMessage);

    bool shouldDisplayMenu = true;
    do {
        cout << "Menu" << endl << "----" << endl;
        cout << "1. New Game" << endl;
        cout << "2. Load Game" << endl;
        cout << "3. Play vs. AI" << endl; 
        cout << "4. Options" << endl;
        cout << "5. Credits (Show student information)" << endl;
        cout << "6. Quit" << endl << endl;
        
        istringstream iss (promptUser());
        int selection = 0;
        iss >> selection;

        cout << endl;

        if (iss.fail()) {
            if (iss.eof()) {
                shouldDisplayMenu = false;
            } else {
                cout << "Invalid Input. Please enter a number from 1-6."
                     << endl << endl;
            }
        } else {
            if (selection == 1) {
                startNewGame();
                shouldDisplayMenu = false;       
            } else if (selection == 2) {
                loadGame();
                shouldDisplayMenu = false;
            } else if (selection == 3) {
                startAIGame();
            } else if (selection == 4) {
                showOptions();
            } else if (selection == 5) {
                showCredits();
            } else if (selection == 6) {
                shouldDisplayMenu = false;
            } else {
                cout << "Sorry, that isn't an option. "
                     << "Please enter a number from 1-6." << endl;
            }
        }
    } while (shouldDisplayMenu);

    LinkedList* list = new LinkedList();
    delete list;

    return EXIT_SUCCESS;
}

std::vector<string> splitString(string inputString, string delimiter) {
    std::vector<string> tokens;

    // Test whether or not there is at least one occurence of delimiter in the
    // string
    if (inputString.find(delimiter) != string::npos) {
        int startpos = 0;
        while (inputString.find(delimiter, startpos) != string::npos) {
            int delimiterLoc = inputString.find(delimiter, startpos);
            int tokenLength = delimiterLoc - startpos;

            tokens.push_back(inputString.substr(startpos, tokenLength));

            startpos = delimiterLoc + delimiter.size();
        }

        // Account for the remaining substring after the last delimiter
        tokens.push_back(inputString.substr(startpos));
    } else {
        // Delimiter is not in string, so add only the original string to the
        // vector
        tokens.push_back(inputString);
    }

    return tokens;
}

bool validateTile(char colour, int shape) {
    char colorsArray[NUM_COLOURS] {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE};

    bool colourIsValid = false;
    for (char c : colorsArray) {
        if (colour == c) {
            colourIsValid = true;
        }
    }

    bool shapeIsValid = shape >= CIRCLE && shape <= CLOVER;

    return colourIsValid && shapeIsValid;
}