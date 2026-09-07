#pragma once

#include "Render.h"


struct Game {
	struct Bounds {
		Bounds (glm::vec2 p1, glm::vec2 p2);
		Bounds (float x1, float y1, float x2, float y2);

		glm::vec2 p1,p2;

		bool collides (Bounds bounds);
	};

	struct Player {
		Player (Game& game);
		Game& game;

		glm::vec2 pos;
		glm::vec2 vel;

		Render::TickFunc movement;
		Render::TickFunc drawTick;

		Render::Mesh mesh;
		glm::vec3 color;


		std::string getName () const { return name; };
		float getTime () const { return timeLeft; };
		void subtractTime (float time) {
			timeLeft-= time;
			if (timeLeft <= 0) game.playerDied(this);
		};
		
		private:
			std::string name;
			float timeLeft;
	};

	struct Obstacle {
		Obstacle (Game& game, Bounds bounds);
		Render::Mesh mesh;
		Game& game;
	};

	Game(int width, int height);

	bool gameRunning = true;

	Render render;
	Render::Scene scene;

	Render::Camera::Orthographic camera;
	std::vector<Render::Shader> shaders;
	Render::Mesh background;

	Render::TickFunc drawTick;

	Player player1;
	Player player2;

	std::vector<Obstacle> obstacles;

	void respawnPlayers ();
	void playerDied (Player* p);
};