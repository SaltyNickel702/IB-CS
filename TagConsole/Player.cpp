#include "Player.h"
#include <iostream>
#include <algorithm>
#include <format>
#include <map>
#include <cmath>

using namespace std;


namespace {
	map<string,int> colorMap = {
		{"black", 40},
		{"red", 41},
		{"green", 42},
		{"yellow", 43},
		{"blue", 44},
		{"magenta", 45},
		{"cyan", 46},
		{"white", 47}
	};
}

Player::Player(Game* g) : game(g), sprite(1,1,1) {
	UI::Sprite::ASCII icon('#');

	string s;
	cout << "Enter Player's name: ";
	getline(cin,s);
	name = s;

	bool lp = true;
	while (lp) {
		cout << "Choose one of the following colors: \n\t";
		for (auto &[key,value] : colorMap) cout << key << "\t";
		cout << endl << ": ";
		getline(cin,s);
		auto it = find(colorMap.begin(),colorMap.end(),s);
		if (it == colorMap.end()) {
			cout << s << " was not recognized." << endl;
			continue;
		}
		icon.color = format("\033[{};{}m",to_string(colorMap[s] + 50),to_string(colorMap[s]));
		break;
	}
	
	sprite.map.at(0).at(0) = icon;
	sprite.link(game->ui);
	game->players.push_back(this);
}
Player::~Player() {
	auto me = find(game->players.begin(),game->players.end(),this);
	if (me != game->players.end()) game->players.erase(me);

	if (game->tagger == this) game->tagger = game->players.at(rand() % game->players.size());
}

bool Player::move (dir d) {
	if (health <= 0) return;
	array<int,2> nPos = pos;
	switch (d) {
		case dir::u:
			nPos.at(1)--;
			break;
		case dir::d:
			nPos.at(1)++;
			break;
		case dir::l:
			nPos.at(0)--;
			break;
		case dir::r:
			nPos.at(0)++;
			break;
	}
	for (Game::Wall *w : game->walls) {
		if (w->collides(nPos)) return false;
	}
	pos = nPos;
	return true;
}
void Player::increaseScore() {
	if (health <= 0) return;
	if (game->tagger == this) return;

	array<int,2> p = game->tagger->pos;
	
	float dist = sqrt(pow(p.at(0) - pos.at(0),2) + pow(p.at(1) - pos.at(1),2));
	score+= floor(max(1.0f,10 * pow(dist,-4/5.0f)));
}
void Player::damage() {
	health--;

	if (health <= 0) {
		sprite.map.at(0).at(0).color = "\033[91;40m";
	}
}