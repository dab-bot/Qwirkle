#ifndef ASSIGN2_TILE_H
#define ASSIGN2_TILE_H

#include <string>

// Define a Colour type
typedef char Colour;

// Define a Shape type
typedef int Shape;

class Tile {
public:
    // Constructors & destructor
    Tile(Colour colour, Shape shape);
    Tile(std::string tileSTR);
    Tile(Tile& other);
    Tile(Tile&& other);
    ~Tile();

    // Getters & setters
    Colour getColour();
    void setColour(Colour newColour);
    Shape getShape();
    void setShape(Shape newShape);

    // Get [letter][number] string representation of the tile
    std::string toString();

    // Check if a tile's shape and colour exactly match those of another tile
    bool equals(Tile t);

    // Check if a tile has at least one blank value
    bool hasBlankValue();

private:
   Colour colour;
   Shape shape;
};

#endif // ASSIGN2_TILE_H
