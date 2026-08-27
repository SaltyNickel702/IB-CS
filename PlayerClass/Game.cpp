#include "Game.h"
#include <iostream>
#include <array>


using namespace std;

Game::Game (int w, int h) : ui(w,h) {
    // Create border walls
    Wall _w1(this,array<int,2>{0,0},array<int,2>{w-1,0});
    Wall _w2(this,array<int,2>{0,0},array<int,2>{w-1,0});
    Wall _w3(this,array<int,2>{0,0},array<int,2>{w-1,0});
    Wall _w4(this,array<int,2>{0,0},array<int,2>{w-1,0});
}