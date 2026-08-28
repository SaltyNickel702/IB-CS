#include "Game.h"
#include <iostream>
#include <cmath>
#include <ctime>

using namespace std;

Game::Wall::Wall (Game* g, std::array<int,2> P1, std::array<int,2> P2) : dim{abs(P2.at(0)-P1.at(0)), abs(P2.at(1)-P1.at(1))}, sprite(abs(P2.at(0)-P1.at(0)) + 1,abs(P2.at(1)-P1.at(1)) + 1) {    
    if (P1.at(0) > P2.at(0)) { p1 = P2; p2 = P1;   
    } else { p1 = P1; p2 = P2; }


    bool negM = (p2.at(1) < p1.at(1));
    float dx = dim.at(0);
    float dy = dim.at(1);
    
    if (dx >= dy) {
        float m = (dx == 0) ? 0 : dy / dx;
        for (int x = 0; x <= dx; x++) {
            int y = negM ? (dim.at(1) - static_cast<int>(round(m * x))) 
                         : static_cast<int>(round(m * x));
            pts.push_back(std::array<int,2>{x, y});
        }
    } else {
        float m = dx / dy;
        for (int y = 0; y <= dy; y++) {
            int x = static_cast<int>(round(m * y));
            int final_y = negM ? (dim.at(1) - y) : y;
            pts.push_back(std::array<int,2>{x, final_y});
        }
    }

    sprite.pos.at(0) = p1.at(0);
    sprite.pos.at(1) = min(p1.at(1), p2.at(1));

    for (std::array<int,2> o : pts) {
        int x = o.at(0);
        int y = o.at(1);
        if (x >= 0 && x <= dim.at(0) && y >= 0 && y <= dim.at(1)) {
            sprite.map.at(y).at(x) = UI::Sprite::ASCII('.',"\033[0;47m");
        }
    }

    sprite.link(g->ui);
}
bool Game::Wall::collides (array<int,2> p) {
    //check bounds before math
    if (p.at(0) < p1.at(0) || p.at(0) > p2.at(0) || p.at(1) < min(p1.at(1),p2.at(1)) || p.at(1) > max(p1.at(1),p2.at(1))) return false;

    for (array<int,2> o : pts) {
        if (o.at(0) == p.at(0) && o.at(1) == p.at(1)) return true;
    }
    return false;
}

Game::Game (int w, int h) : ui(w,h), dim{w,h} {
    // Create border walls
    // cout << "Generating Borders" << endl;
    Wall _w1(this,array<int,2>{0,0},array<int,2>{w-1,0}); //Slightly oversize, doesn't matter
    Wall _w2(this,array<int,2>{w-1,0},array<int,2>{w-1,h-1});
    Wall _w3(this,array<int,2>{w-1,h-1},array<int,2>{0,h-1});
    Wall _w4(this,array<int,2>{0,h-1},array<int,2>{0,0});

    // Wall* w1 = new Wall(this, array<int,2>{5,25}, array<int,2>{15,5});
    srand(time(0));

    cout << "Generating Walls" << endl;
    genWalls(20, array<int,2>{20,7}); //rand() % 3 + 2

    ui.display();
}
void Game::genWalls (int count, array<int,2> size) {
    srand(time(0));

    while (count--) {
        array<int,2> rSize = {rand() % size.at(0), rand() % size.at(1)};
        if (rSize.at(0) == 0 && rSize.at(1) == 0) { count++; continue; } //retry
        rSize.at(1) *= (rand() % 2 == 0 ? 1 : -1);


        array<int,2> sPos = {rand() % (dim.at(0) - rSize.at(0)), rand() % (dim.at(1) - rSize.at(1))};
        if (rSize.at(1) < 0) sPos.at(1) += -rSize.at(1);

        
        array<int,2> toPos = {sPos.at(0) + rSize.at(0), sPos.at(1) + rSize.at(1)};

        // cout << "\nWall Coords: " << endl;
        // cout << sPos.at(0) << '\t' << sPos.at(1) << endl;
        // cout << toPos.at(0) << '\t' << toPos.at(1) << endl;

        // cout << "Construct:" << endl;
        walls.push_back(new Wall(this, sPos, array<int,2>{sPos.at(0) + rSize.at(0), sPos.at(1) + rSize.at(1)}));
    }
}