#ifndef GAME_H
#define GAME_H

#include "UI.h"
#include "Player.h"
#include <vector>

struct Game {
    struct Wall {
        Wall (Game* game, int p1[2], int p2[2]);

        bool collides(int position[2]);

        UI::Sprite sprite;
        
        private:
            int p1[2];
            int p2[2];
    };

    Game (int w, int h);

    void genWalls (uint count, uint maxSize);

    void turn();
    Player* tagger;

    private:
        UI ui;

        std::vector<Player*> players;
        std::vector<Wall> walls;
};

#endif