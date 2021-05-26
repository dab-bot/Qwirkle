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
    std::string stringRepr = "\033[" + colourString(colour) + colour + std::to_string(shape) + "\033[m"; 
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
    std::cout << "hello" << std::endl;
    std::cout << "this colour is " + c << std::endl;
    switch (c)  {
        case 'R': 
            code = "48;5;196m";
        case 'O': 
            code = "48;5;202m";
        case 'Y': 
            code = "48;5;3m";
        case 'G': 
            code = "48;5;34m";
        case 'B': 
            code = "48;5;21m";
        case 'P': 
            code = "48;5;129m";
        default: 
            code = "48;5;0m"; //default colour to use no identifier
        return code;
    }
}