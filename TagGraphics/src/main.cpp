#include "Game.h"

using namespace std;

int main () {
	glm::vec2 res(1,.5f);
	res*= 2000;
	Game g(res.x,res.y);

	thread* t = g.render.Thread;

	if (t->joinable()) t->join();
	return 0;
}