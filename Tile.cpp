#include <cctype>

#include "Tile.h"
#include "TileCodes.h"
#include <iostream>

Tile::Tile(Colour colour, Shape shape) :
    colour(std::toupper(colour)),
    shape(shape)
{}

// This is a stupid bodge
Tile::Tile(std::string tileSTR)
{
    colour = tileSTR.at(0);
    shape = std::stoi(tileSTR.substr(1,1));
}

Tile::Tile(Tile& other) :
    colour(other.colour),
    shape(other.shape)
{}

Tile::Tile(Tile&& other) :
    colour(other.colour),
    shape(other.shape)
{}

Tile::~Tile() {
    // No manual deletes necessary
}

Colour Tile::getColour() {
    return colour;
}

void Tile::setColour(Colour newColour) {
    colour = std::toupper(newColour);
}

Shape Tile::getShape() {
    return shape;
}

void Tile::setShape(Shape newShape)  {
    shape = newShape;
}

std::string Tile::toString() {
    std::string stringRepr = colour + std::to_string(shape); 
    return stringRepr;
}

std::string Tile::toConsoleString() {
    std::string stringRepr = colourString(colour) + colour + std::to_string(shape) + "\033[m"; 
    return stringRepr;
}

bool Tile::equals(Tile t) {
    return t.getColour() == colour && t.getShape() == shape;
}

bool Tile::hasBlankValue() {
    return colour == NO_COLOUR || shape == NO_SHAPE;
}

std::string Tile::colourString(char c) {
    std::string code = "";
    if (c == 'R'){
        code = "\033[48;5;196;38;5;15m";
    }else if(c == 'O') {
        code = "\033[48;5;202;38;5;0m";
    }else if(c == 'Y') {
        code = "\033[48;5;220;38;5;0m";
    }else if(c == 'G') {
        code = "\033[48;5;34;38;5;15m";
    }else if(c == 'B') {
        code = "\033[48;5;21;38;5;15m";
    }else if(c == 'P') {
        code = "\033[48;5;129;38;5;15m";
    }
    return code;
}