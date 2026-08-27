#include "UI.h"
#include <format>
#include <algorithm>

using namespace std;

UI::UI (int width, int height) : scrn(height, vector<string>(width, " ")) {
	dim[0] = width;
	dim[1] = height;

	for (int i = 0; i <= height; i++) cout << endl;

	fillChar = ' ';
	display();
}
UI::Sprite::Sprite (int w, int h, int p) : priority(p), map(h, vector<ASCII>(w,' ')) {
	dim[0] = w;
	dim[1] = h;

	linkedUI = nullptr;
	pos[0] = 0;
	pos[1] = 0;
}
void UI::addSprite(Sprite* s) {
	if (s->linkedUI) {
		UI* o = s->linkedUI;
		auto it = find(o->sprites.begin(),o->sprites.end(), s);
		if (it != o->sprites.end()) o->sprites.erase(it);
	}
	s->linkedUI = this;
	for (int i = 0; i < sprites.size(); i++) {
		if (s->priority < sprites.at(i)->priority) {
			sprites.insert(sprites.begin() + i, s);
			return;
		}
	}
	sprites.push_back(s);
}

void UI::Sprite::importFromString (string s) {
	int cCount = 0;
	for (int y = 0; y < dim[1]; y++) {
		for (int x = 0; x < dim[0]; x++) {
			map[y][x] = s.at(cCount);

			cCount++;
			if (cCount >= s.size()) return;
		}
	}
}

void UI::display () {
	updateScrn();

	std::cout << format("\033[{}A\033[0J",dim[1]+1);
	string s = "";
	for (int y = 0; y < dim[1]; y++) {
		string ss;
		for (int x = 0; x < dim[0]; x++) {
			ss += scrn.at(y).at(x);
		}
		s += ss + "\n";
	}
	cout << s << endl;
}

void UI::updateScrn() {
	for (vector<string> &v : scrn) {
		for (string &c : v) c = fillChar;
	}

	for (Sprite* s : sprites) {
		vector<vector<Sprite::ASCII>> &m = s->map;
		for (int y0 = 0; y0 < m.size(); y0++) {
			int y = y0 + s->pos[1];
			if (y < 0 || y >= dim[1]) continue;

			for (int x0 = 0; x0 < m.at(0).size(); x0++) {
				int x = x0 + s->pos[0];

				if (x < 0 || x >= dim[0]) continue;

				Sprite::ASCII c = m.at(y0).at(x0);

				if (c.c != ' ') scrn.at(y).at(x) = c.color + c.c;
			}
		}
	}
}