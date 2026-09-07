#include "Game.h"

using namespace std;

/*

TickFunc priorities
0: draw background, draw obstacles, draw players
2: process player tagging
3: Player movement

-1: Player Death

Z levels:
Background at 3
Obstacles at 2
UI? at 1
Players at 0

*/

Game::Game (int w, int h) : render(w,h,"TAG"), scene("scene1",render), background(render), drawTick(render), player1(*this), player2(*this) {
	Render::Shader rectShader("vert.glsl","frag.glsl",render);
}

Game::Player::Player (Game& g) : game(g), movement(game.render), drawTick(game.render), mesh(game.render) {
	movement.preferedPriority.store(3);
	movement.permanent.store(true);


	drawTick.preferedPriority.store(0);
	drawTick.permanent.store(true);
	drawTick.f = [&]() {

	};
	

	game.scene.addAsset(&movement);
	game.scene.addAsset(&drawTick);
	game.scene.addAsset(&mesh);
}