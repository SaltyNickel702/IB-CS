#include "Game.h"
#include <iostream>
#include <cmath>
#include <ctime>
#include <map>

using namespace std;

namespace {
    float distance (array<int,2> p1, array<int,2> p2) { return sqrt(pow(p2.at(0) - p1.at(0), 2) + pow(p2.at(1) - p1.at(1), 2)); };
}

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

Game::Game (int w, int h) : ui(w,h), dim{w,h}, tagger(nullptr) {
    //Create players
    Player* p1 = new Player(this);
    Player* p2 = new Player(this);
    tagger = players.at(rand() % players.size());
    cout << "Tagger: " << tagger->getName() << endl;
    
    // Create border walls
    Wall _w1(this,array<int,2>{0,0},array<int,2>{w-1,0});
    Wall _w2(this,array<int,2>{w-1,0},array<int,2>{w-1,h-1});
    Wall _w3(this,array<int,2>{w-1,h-1},array<int,2>{0,h-1});
    Wall _w4(this,array<int,2>{0,h-1},array<int,2>{0,0});

    // Generate Wall Maze
    srand(time(0));
    genWalls(20, array<int,2>{20,7}); //rand() % 3 + 2

    
    randomizePlayers();

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
void Game::turn () {
    bool reset = false;
    Player* taggedPlayer = nullptr;
    for (Player* p : players) {
        Restart:

        if (p->getHealth() <= 0) continue;
        char inp = UI::getKeyInput();
        bool goodMove;
        if (inp == 'w' || inp == 'i') { goodMove = p->move(Player::dir::u);
        } else if (inp = 's' || inp == 'k') { goodMove = p->move(Player::dir::d);
        } else if (inp = 'a' || inp == 'j') { goodMove = p->move(Player::dir::l);
        } else if (inp = 'd' || inp == 'l') { goodMove = p->move(Player::dir::r);
        } else { goto Restart; } // Does this work?

        if (!goodMove) {
            p->damage();
        }

        if (tagger != p) {
            if (tagger->pos.at(0) == p->pos.at(0) || tagger->pos.at(1) == p->pos.at(1)) {
                p->damage();
                reset = true;
                taggedPlayer = p;
            } else {
                p->increaseScore();
            }
        }
    }
    if (reset) {
        tagger = taggedPlayer;
        randomizePlayers();
    }
}
void Game::randomizePlayers() {
    auto findSpot = [&](Player* p) {
        while (true) {
            array<int,2> r = {rand() % (dim.at(0) - 2) + 1, rand() % (dim.at(1) - 2) + 1};
            bool collides = false;
            for (Wall* w : walls) if (w->collides(r)) collides = true;
            if (tagger != p && distance(r,tagger->pos) < 2) collides = true; //Can't be next to another player

            if (!collides) break;
        }
    };
    findSpot(tagger); // Tagger First
    for (Player* p : players) {
        if (p != tagger) findSpot(p);
    }
}

Game::~Game () {
    for (Wall* w : walls) delete w;
    for (Player* p : players) delete p;
}