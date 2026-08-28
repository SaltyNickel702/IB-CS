#ifndef UI_H
#define UI_H

#include <iostream>
#include <vector>
#include <string>
#include <array>

struct UI {
	UI(int width, int height);

	struct Sprite {
		struct ASCII {
			ASCII (char character, std::string colorCode) : c(character), color(colorCode) {};
			ASCII (char character) : ASCII(character,"\033[0m") {}; //0m is default color
			ASCII () : ASCII(' ') {};

			std::string color;
			char c;
		};

		Sprite(int width, int height, int priority);
		Sprite(int width, int height) : Sprite(width, height, 0) {};


		const int priority;
		std::array<int, 2> pos; //from top left
		std::vector<std::vector<ASCII>> map; //contains sprite image | ' ' is empty

		void importFromString (std::string String, std::string colorCode); // Populates Sprite::map from a string
		void importFromString (std::string String) { importFromString(String, std::string("\033[0m")); }
		// static void importFromFile (int priority); For future update
		
		void link (UI &ui) {ui.addSprite(this);};
		UI* linkedUI;
		
		private:
			std::array<int, 2> dim;
			std::vector<std::vector<bool>> loc;
	};

	void display();
	void addSprite (Sprite* sprite);

	std::string fillChar; // String allows for coloring

	private:
		std::array<int, 2> dim;
		std::vector<std::vector<std::string>> scrn;
		std::vector<Sprite*> sprites;

		void updateScrn();
};


#endif