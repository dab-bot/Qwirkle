#ifndef ASSIGN2_LINKEDLIST_H
#define ASSIGN2_LINKEDLIST_H

#include "Node.h"

class LinkedList {
public:

    LinkedList();
    LinkedList(LinkedList& other);
    ~LinkedList();

    std::string toString();
    std::string toConsoleString();

    // we won't need addFront or insert, but do them for practise
    void addFront(Tile* tile);
    void addBack(Tile* tile);

    void removeFront();
    void removeBack();
    void remove(int index);
    void clear();
    bool contains(Tile* t);

    Tile* getFront();
    Tile* getBack();
    Tile* get(int index);

    int findTileIndex(Tile* tile);
    int getSize();

    bool findAndRemove(Tile* tile);

private:
    Node* head;
    Node* tail;
    int size;
};

#endif // ASSIGN2_LINKEDLIST_H
