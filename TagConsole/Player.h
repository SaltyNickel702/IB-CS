#ifndef PLAYER_H
#define PLAYER_H

#include "UI.h"
#include "Game.h"
#include <string>
#include <vector>

struct Player {
    enum dir {l, r, u, d};

    Player (Game* game);

    void move (Player::dir direction);

    const Game* game;
    const UI::Sprite sprite;

    private:
        int pos[2];
        int health = 3;
        float score; //increases by 1 / distance to tagger every turn
        std::string name;
};

#endif