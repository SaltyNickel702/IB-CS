#include "Game.h"

using namespace std;

int main () {
	Game g(1000,500);

	thread* t = g.render.Thread;

	if (t->joinable()) t->join();
	return 0;
}