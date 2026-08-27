#ifndef UI_H
#define UI_H

#include <iostream>
#include <vector>
#include <string>

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
		int pos[2]; //from top left
		std::vector<std::vector<ASCII>> map; //contains sprite image | ' ' is empty

		void importFromString (std::string String); // Populates Sprite::map from a string
		// static void importFromFile (int priority); For future update
		
		void link (UI &ui) {ui.addSprite(this);};
		UI* linkedUI;
		
		private:
			int dim[2];
	};

	void display();
	void addSprite (Sprite* sprite);

	char fillChar;

	private:
		int dim[2];
		std::vector<std::vector<std::string>> scrn;
		std::vector<Sprite*> sprites;

		void updateScrn();
};


#endif