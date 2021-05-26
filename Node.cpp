#include "Node.h"

Node::Node(Tile* pTile, Node* pPrevious, Node* pNext)
{
    tile = new Tile(*pTile);
    previous = pPrevious;
    next = pNext;
}

Node::Node(Node& other)
{
    tile = new Tile(*other.tile);
    previous = other.previous;
    next = other.next;
}

Node::~Node()
{
    delete tile;
}