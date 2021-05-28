#include "Player.h"
#include <iostream>

#define MAX_PLAYER_HAND_SIZE 6

// Construct Player object with specified name and score of 0
Player::Player(string n) :
    name(n),
    score(0),
    aiStatus(false)
{
    hand = new LinkedList();
}

Player::Player(bool ai) :
    name("QuirkleBot"),
    score(0),
    aiStatus(true)
{
    hand = new LinkedList();
}

Player::~Player() {
    delete hand;
}

Player::Player(Player& other) {
    name = other.getName();
    score = other.getScore();
    aiStatus = other.getAIStatus();
    hand = new LinkedList(*other.getHand());
}

string Player::getName() {
    return name;
}

int Player::getScore() {
    return score;
}

void Player::setScore(int s) {
    score = s;
}

void Player::setAIStatus(bool s) {
    aiStatus = s;
}

bool Player::getAIStatus() {
    return aiStatus;
}

LinkedList* Player::getHand() {
    return hand;
}

/*
 * checks if player's hand contains specified tile
 * returns false if tile does not exist in player hand
 */
bool Player::hasTile(Tile* t) {
    return hand->contains(t);
}

/* 
 * removes specified string representation of tile from player hand if it exists
 * returns false if tile does not exist in hand
 */
bool Player::removeFromHand(string s) {
    bool tileExistence = false;
    int index = -1;
    for (int i = 0; i < hand->getSize(); ++i) {
        // this relies on `s` being correctly formatted/validated after input to coincide with tilecode definition
        if (hand->get(i)->toString().compare(s) == 0) {
            index = i;
            tileExistence = true;
        }
    }
    if (tileExistence) {
        // removes last instance of `s` in player hand (can be maximum of 2)
        hand->remove(index);
    }
    return tileExistence;
}

/*
 * removes specified tile if it exists in player's hand
 * returns false if tile does not exist in hand
 */
bool Player::removeFromHand(Tile* t) {
    bool successful = false;

    if (hasTile(t)) {
        int i = hand->findTileIndex(t);
        hand->remove(i);
        successful = true;
    }

    return successful;
}

/* 
 * adds to hand only if hand is not currently full (6 tile limit)
 * returns false on unsuccessful addition of specified tile
 */
bool Player::addToHand(Tile* t) {
    bool successful = false;

    if (hand->getSize() < MAX_PLAYER_HAND_SIZE) {
        hand->addBack(t);
        successful = true;
    }

    return successful;
}

string Player::serialise() {
    string str = name + "\n";
    str += ((aiStatus)?"AI\n":"HUMAN\n");
    str += std::to_string(score) + "\n";
    str+= hand->toString() + "\n";
    return str;
}