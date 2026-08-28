#include "UI.h"
#include <format>
#include <algorithm>

using namespace std;

UI::UI (int width, int height) : scrn(height, vector<string>(width, " ")) {
	dim.at(0) = width;
	dim.at(1) = height;

	for (int i = 0; i <= height; i++) cout << endl;

	fillChar = ' ';
	display();
}
UI::Sprite::Sprite (int w, int h, int p) : priority(p), map(h, vector<ASCII>(w,' ')) {
	dim.at(0) = w;
	dim.at(1) = h;

	linkedUI = nullptr;
	pos.at(0) = 0;
	pos.at(1) = 0;
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

void UI::Sprite::importFromString (string s, string cc) {
	int cCount = 0;
	for (int y = 0; y < dim.at(1); y++) {
		for (int x = 0; x < dim.at(0); x++) {
			ASCII a(s.at(cCount), cc);
			map[y][x] = a;

			cCount++;
			if (cCount >= s.size()) return;
		}
	}
}

void UI::display () {
	updateScrn();

	cout << format("\033[{}A\033[0J",dim.at(1)+1);
	string s = "";
	for (int y = 0; y < dim.at(1); y++) {
		string ss;
		for (int x = 0; x < dim.at(0); x++) {
			string px = scrn.at(y).at(x);
			ss += px + "\033[0m";
		}
		s += ss + "\n";
	}
	cout << s << "\033[0m" << endl;
}

void UI::updateScrn() {
	for (vector<string> &v : scrn) {
		for (string &s : v) s = fillChar;
	}

	for (Sprite* s : sprites) {
		vector<vector<Sprite::ASCII>> &m = s->map;
		for (int y0 = 0; y0 < m.size(); y0++) {
			int y = y0 + s->pos.at(1);
			if (y < 0 || y >= dim.at(1)) continue;

			for (int x0 = 0; x0 < m.at(0).size(); x0++) {
				int x = x0 + s->pos.at(0);

				if (x < 0 || x >= dim.at(0)) continue;

				Sprite::ASCII c = m.at(y0).at(x0);

				if (c.c != ' ') scrn.at(y).at(x) = c.color + c.c;
			}
		}
	}
}