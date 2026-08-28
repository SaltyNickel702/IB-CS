#ifndef GAME_H
#define GAME_H

#include "UI.h"
// #include "Player.h"
#include <vector>
#include <array>

struct Game {
    struct Wall {
        Wall (Game* game, std::array<int,2> Pos1, std::array<int,2> Pos2);

        bool collides(std::array<int,2> position);

        UI::Sprite sprite;
        
        private:
            std::array<int,2> p1;
            std::array<int,2> p2;
            std::array<int,2> dim;
            std::vector<std::array<int,2>> pts;
    };

    Game (int w, int h);

    void genWalls (int count, std::array<int,2> maxSize);

    void turn();
    // Player* tagger;

    private:
        UI ui;
        std::array<int,2> dim;

        // std::vector<Player*> players;
        std::vector<Wall*> walls;
};

#endif