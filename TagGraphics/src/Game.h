#pragma once

#define GAME_CAM true

#include "Render.h"
#include <array>


struct Game {
	struct Bounds {
		Bounds (glm::vec2 p1, glm::vec2 p2);
		Bounds (float x1, float y1, float x2, float y2) : Bounds(glm::vec2(x1,y1), glm::vec2(x2,y2)) {};

		glm::vec2 p1,p2;

		void translate(glm::vec2 amount);

		bool collides (Bounds bounds);

		std::vector<glm::vec2> checkPnts;
		void genPoints (float dist); //distance between each point (kinda)
	};

	struct Player {
		Player (Game& game);
		Game& game;

		glm::vec2 pos;
		glm::vec2 vel;

		Bounds getBounds();

		Render::TickFunc movement;
		Render::TickFunc drawTick;

		Render::Mesh mesh;
		glm::vec3 color;

		bool grounded = false;
		bool onWall = false;
		int wallEjectSide;
		bool usedJump = false;
		bool usedWallJump = false;

		std::array<int,4> movementKeys; //w,a,s,d using GLFW_KEY_[key]

		std::string getName () const { return name; };
		float getTime () const { return timeLeft; };
		void subtractTime (float time) {
			timeLeft-= time;
			if (timeLeft <= 0) game.playerDied(this);
		};
		
		private:
			std::string name;
			float timeLeft;

			glm::vec2 dim;
	};

	struct Obstacle {
		Obstacle (Game& game, Bounds bounds);
		Render::Mesh* mesh;
		Game& game;
		Bounds bounds;
		glm::vec3 color;
	};

	Game(int width, int height);

	bool gameRunning = true;

	Render render;
	Render::Scene scene;

	glm::vec2 scrnDim;
	glm::vec2 worldDim;

	#if GAME_CAM
	Render::Camera::Orthographic camera;
	#else
	Render::Camera::Perspective camera;
	#endif
	std::map<std::string,Render::Shader*> shaders;
	Render::Mesh background;

	Render::TickFunc drawTick;

	Player player1;
	Player player2;
	Player* tagger;

	std::vector<Obstacle> obstacles;

	void respawnPlayers ();
	void playerDied (Player* p);
};