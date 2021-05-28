#ifndef ASSIGN2_BOARD_H
#define ASSIGN2_BOARD_H

#include <string>
#include <vector>

#include "Tile.h"

// Maximum board size for Milestone 2 is 26x26
#define BOARD_MAX_DIM 26

class Board {
public:
    // Constructors/Destructor
    Board();
    Board(Board& other);
    ~Board();

    // Board dimension getters
    int getHeight();
    int getWidth();

    // Get a pointer to a copy of the tile at a position on the board, or a
    // nullptr if there is no tile at that position
    Tile* tileAt(char row, int col);
    Tile* tileAt(int row, int col);

    // Place a tile on the board. Return true if successful, otherwise false.
    // Copies the given tile's shape and colour into the correct board position
    bool placeTile(Tile& tile, char row, int col);

    // Get the board in string format, for printing to the console as plain text
    std::string toString();
    // Get a board position in string format from a row and column index
    std::string positionString(int row, int col);
    // Get the board in string format, for printing to the console in colour
    std::string toConsoleString();

    // Get the board in string format, for writing to a game save file
    std::string serialise();

private:
    std::vector<std::vector<Tile*>> board;

    // Convert the char used to identify a row to the corresponding index in
    // the board vector
    //
    // Expects a character from A-Z (can be uppercase or lowercase)
    int rowCharToIndex(char row);

    // Convert the index of a row in the board vector to the character used to
    // identify it. (Essentially the reverse of rowCharToIndex())
    char rowIndexToChar(int row);
};

#endif // ASSIGN2_BOARD_H