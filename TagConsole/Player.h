#ifndef PLAYER_H
#define PLAYER_H

#include "UI.h"
// #include "Game.h"
class Game;
#include <string>
#include <vector>
#include <array>

struct Player {
    enum dir {l, r, u, d};

    Player (Game* game);

    bool move (Player::dir direction); // good or bad move
    bool move (std::array<int,2> position); // good or bad move
    void increaseScore ();
    void damage();
    
    
    std::string getName() { return name; }; // This is redundant, but required for assignment
    int getHealth() { return health; };
    int getScore() { return score; };

    std::array<int,2> pos;

    ~Player ();
    private:
        int health = 3;
        int score = 0; //increases by 10 / distance to tagger every turn
        std::string name;
        UI::Sprite sprite;
        Game* game;
};

#endif