#include <cctype>
#include <stdexcept>

#include "Board.h"
#include "TileCodes.h"

#define ASCII_VALUE_A 65
#define COLUMN_WIDTH 3

Board::Board() {
    // Initialise board with rows of "blank" tiles to represent empty spaces
    for (int i = 0; i < BOARD_MAX_DIM; ++i) {
        std::vector<Tile*> tileRow;

        for (int j = 0; j < BOARD_MAX_DIM; ++j) {
            // Tiles have a move constructor, so there are no memory leaks here
            tileRow.push_back(new Tile(NO_COLOUR, NO_SHAPE));
        }

        board.push_back(tileRow);
    }
}

Board::Board(Board& other) {
    // Make copies of all tiles in the source board
    for (unsigned int i = 0; i < other.board.size(); ++i) {
        std::vector<Tile*> tileRow;

        for (unsigned int j = 0; j < other.board.at(i).size(); ++j) {
            Tile* currentTile = other.board.at(i).at(j);
            tileRow.push_back(new Tile(*currentTile));
        }

        board.push_back(tileRow);
    }
}

Board::~Board() {
    for (unsigned int i = 0; i < board.size(); ++i) {
        for (unsigned int j = 0; j < board.at(i).size(); ++j) {
            delete board.at(i).at(j);
        }
    }
}

int Board::getHeight() {
    return (int) board.size();
}

int Board::getWidth() {
    // Gets the length of the first row, which, in normal operation, should
    // have a length equal to that of all other rows.
    return (int) board.at(0).size();
}

Tile* Board::tileAt(char row, int col) {
    Tile* returnPointer = nullptr;
    Tile* boardPos = nullptr;

    try {
        boardPos = board.at(rowCharToIndex(row)).at(col);

        // returnPointer stays as null if there is no tile at this position
        if (!boardPos->hasBlankValue()) {
            returnPointer = new Tile(*boardPos);
        }
    } catch (std::out_of_range& e) {
        // returnPointer is already set to nullptr, so no need to do anything
        // here
    }  

    return returnPointer;
}

Tile* Board::tileAt(int row, int col) {
    Tile* returnPointer = nullptr;
    Tile* boardPos = nullptr;

    try {
        boardPos = board.at(row).at(col);

        // returnPointer stays as null if there is no tile at this position
        if (!boardPos->hasBlankValue()) {
            returnPointer = new Tile(*boardPos);
        }
    } catch (std::out_of_range& e) {
        // returnPointer is already set to nullptr, so no need to do anything
        // here
    }  

    return returnPointer;
}

bool Board::placeTile(Tile& tile, char row, int col) {
    bool tilePlaced = false;
    Tile* boardPos = nullptr;

    try {
        boardPos = board.at(rowCharToIndex(row)).at(col);

        // Modify the existing tile in the board so that its colour and shape
        // match that of the given tile
        boardPos->setColour(tile.getColour());
        boardPos->setShape(tile.getShape());

        tilePlaced = true;
    } catch (std::out_of_range& e) {
        // Could not place the tile, tilePlaced remains false
    }

    return tilePlaced;
}

std::string Board::toString() {
    std::string repr = "";

    // Whitespace before column labels
    repr += "   ";

    // Column labels
    for (unsigned int i = 0; i < board.at(0).size(); ++i) {
        std::string colLabel = std::to_string(i);
        repr += colLabel;

        // Add whitespace to align labels with columns
        for (unsigned int j = 0; j < COLUMN_WIDTH - colLabel.length(); ++j) {
            repr += " ";
        }
    }

    repr += "\n";

    // Horizontal line below column labels
    repr += "  -";

    for (unsigned int i = 0; i < board.at(0).size(); ++i) {
        for (int j = 0; j < COLUMN_WIDTH; ++j) {
            repr += "-";
        }
    }

    repr += "\n";

    // Rows
    for (unsigned int i = 0; i < board.size(); ++i) {
        // Row label, represented as a letter
        repr += rowIndexToChar(i);

        // Pipe between the label and the grid
        repr += " |";

        // Append each column in the row
        for (unsigned int j = 0; j < board.at(i).size(); ++j) {
            Tile* tile = board.at(i).at(j);

            // Append the tile only if one exists at that cell
            if (tile != nullptr && !tile->hasBlankValue()) {
                repr += tile->toString();
            } else {
                repr += "  ";
            }

            repr += "|";
        }

        // Last row doesn't have a newline at the end
        if (i < board.size() - 1) {
            repr += "\n";
        }
    }

    return repr;
}

std::string Board::serialise() {
    std::string repr = "";
    bool firstElement = true;

    for (unsigned int row = 0; row < board.size(); ++row) {
        for (unsigned int col = 0; col < board.at(row).size(); ++col) {
            Tile* tile = board.at(row).at(col);

            if (tile != nullptr && !tile->hasBlankValue()) {
                // Precede all but the first tile with ", "
                if (!firstElement) {
                    repr += ", ";
                } else {
                    firstElement = false;
                }

                // Tile is stored in tile@position format
                repr += tile->toString();
                repr += "@";
                repr += rowIndexToChar(row) + std::to_string(col);
            }
        }
    }

    return repr;
}

int Board::rowCharToIndex(char row) {
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

char Board::rowIndexToChar(int row) {
    return (char) row + ASCII_VALUE_A;
}
