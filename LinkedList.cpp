#include "LinkedList.h"

#include <iostream>

LinkedList::LinkedList() {
    head = nullptr;
    tail = nullptr;
    size = 0;
}

// The deep copy constructor and clear methods are adapted from this post:
// https://stackoverflow.com/a/34963321

LinkedList::LinkedList(LinkedList& other) {
    if (other.head == nullptr) {
        head = nullptr;
        tail = nullptr;
        size = 0;
    } else {
        head = new Node(*other.head);
        tail = head;
        size = 1;
        
        Node* tempOther = other.head->next;
        while (tempOther != nullptr) {
            addBack(tempOther->tile);
            tempOther = tempOther->next;
        }
        
    }
}

LinkedList::~LinkedList() {
    if (size != 0) {
        clear();
    }
}

std::string LinkedList::toString() {
    std::string str = "";

    if (size > 0) {
        Node* n = head;
        while (n != nullptr) {
            str += n->tile->toString();

            if (n->next != nullptr) {
                str += ",";
            }

            n = n->next;
        }
    }
    
    return str;
}

std::string LinkedList::toConsoleString() {
    std::string str = "";

    if (size > 0) {
        Node* n = head;
        while (n != nullptr) {
            str += n->tile->toConsoleString();

            if (n->next != nullptr) {
                str += ",";
            }

            n = n->next;
        }
    }
    
    return str;
}

void LinkedList::addFront(Tile* tile) {
    if (size == 0) {
        Node* newNode = new Node(tile, nullptr, nullptr);
        head = newNode;
        tail = newNode;
        size++;
    } else {
        Node* newNode = new Node(tile, nullptr, head);
        head->previous = newNode;
        head = newNode;
        size++;
    }
}

void LinkedList::addBack(Tile* tile) {
    if (size == 0) {
        addFront(tile);
    } else {
        Node* newNode = new Node(tile, tail, nullptr);
        tail->next = newNode;
        tail = newNode;
        size++;
    }
}

void LinkedList::removeFront() {
    if (size == 0) {
        std::cout << "Cannot removeF: this list is empty!" << std::endl;
    } else {
        Node* temp = head;
        head = head->next;
        if (head != nullptr) {head->previous = nullptr;}
        delete temp;
        size--;
    }
}

void LinkedList::removeBack() {
    if (size == 0) {
        std::cout << "Cannot removeB: this list is empty!" << std::endl;
    } else {
        tail = tail->previous;
        delete tail->next;
        tail->next = nullptr;
        size--;
    }
}

void LinkedList::remove(int index) {
    if (size == 0) {
        std::cout << "Cannot remove: this list is empty!" << std::endl;
    } else {
        if (index == 0) {
            removeFront();
        } else if (index == size-1) {
            removeBack();
        } else {
            Node* del = head;
            int counter = 0;

            while (del != nullptr && counter < index) {
                del = del->next;
                counter++;
            }

            del->previous->next = del->next;
            del->next->previous = del->previous;
            delete del;
            size--;
        }
    }
}

void LinkedList::clear() {
    Node* n = head;
    while (n != nullptr) {
        Node* del = n;
        n = n->next;
        delete del;
        size--;
    }

    head = nullptr;
    tail = nullptr;
}

bool LinkedList::contains(Tile* t) {
    bool result = false;
    Node* temp = head;

    while (temp != nullptr) {
        // debugging
        // std::cout << "checking... " << temp->tile->toString() << std::endl;
        if (temp->tile->equals(*t)) {
            result = true;
        }

        temp = temp->next;
    }

    return result;
}

Tile* LinkedList::getFront() {
    Tile* tile = nullptr;

    if (head != nullptr) {
        tile = head->tile;
    } else {
        std::cout << "Cannot getF: this list is empty!" << std::endl;
    }

    return tile;
}

Tile* LinkedList::getBack() {
    Tile* tile = nullptr;

    if (tail != nullptr) {
        tile = tail->tile;
    } else {
        std::cout << "Cannot getB: this list is empty!" << std::endl;
    }

    return tile;
}

Tile* LinkedList::get(int index) {
    Tile* tile = nullptr;

    if (size != 0) {
        Node* n = head;
        int counter = 0;

        while (n != nullptr && counter < index) {
            n = n->next;
            counter++;
        }

        tile = n->tile;
    } else {
        std::cout << "Cannot get: this list is empty!" << std::endl;
    }

    return tile;
}

int LinkedList::findTileIndex(Tile* tile) {
    Node* n = head;
    int index = 0;
    int counter = 0;

    while (n != nullptr && counter < size && !n->tile->equals(*tile)) {
        n = n->next;
        counter++;
    }

    if (n == nullptr) {
        index = -1;
    } else {
        index = counter;
    }

    return index;
}

int LinkedList::getSize() {
    return size;
}

bool LinkedList::findAndRemove(Tile* pTile){
    bool success;
    if (contains(pTile)){
        remove(findTileIndex(pTile));
        success = true;
    }
    else{
        success = false;
    }
    return success;
}