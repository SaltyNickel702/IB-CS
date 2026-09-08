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
	scrnDim = glm::vec2(w,h);
	worldDim = glm::vec2(40,20); //2 : 1

	#if GAME_CAM
		camera.bottom = 0;
		camera.left = 0;
		camera.right = worldDim.x;
		camera.top = worldDim.y;
		camera.near = -1.0f;
		camera.far = 100.0f;
	#else
		camera.pos = glm::vec3(-10, worldDim.y / 2, -20);

		camera.rot = glm::quatLookAtLH(
			glm::normalize(glm::vec3(1,0,1)),
			glm::vec3(0, 1, 0)
		);

		camera.fov = 70.0f;
		camera.near = 0.1f;
		camera.far = 100.0f;
		camera.aspectRatio = 2.0f;
	#endif

	player1.movementKeys = array<int,4>{GLFW_KEY_W,GLFW_KEY_A,GLFW_KEY_S,GLFW_KEY_D};
	player1.color = glm::vec3(0.196, 0.659, 0.322);
	player2.movementKeys = array<int,4>{GLFW_KEY_UP,GLFW_KEY_LEFT,GLFW_KEY_DOWN,GLFW_KEY_RIGHT};
	player2.color = glm::vec3(0.659, 0.196, 0.259);
	

	Render::Shader* rectShader = new Render::Shader("vert.glsl","frag.glsl",render);
	shaders["rect"] = rectShader;
	scene.addAsset(rectShader);

	{ // MARK: Map Creation
		float borderHeight = 1.5;
		vector<Bounds> border {
			Bounds(0,0,borderHeight,worldDim.y),
			Bounds(0,0,worldDim.x,borderHeight),
			Bounds(0,worldDim.y - borderHeight,worldDim.x,worldDim.y),
			Bounds(worldDim.x - borderHeight,0,worldDim.x,worldDim.y)
		};
		for (Bounds b : border) {
			Obstacle o(*this, b);
			o.color = glm::vec3(0.33, 0.24, 0.13);
			obstacles.push_back(o);
			scene.addAsset(obstacles.back().mesh);
		}

		vector<Bounds> interiorLayout { // used ai to generate map layout
            // Central core to break cross-map sightlines
            Bounds(17.0f, 8.0f, 23.0f, 12.0f),
            
            // Top and bottom choke points connecting to the border
            Bounds(19.0f, borderHeight, 21.0f, 6.0f),
            Bounds(19.0f, worldDim.y - 6.0f, 21.0f, worldDim.y - borderHeight),
            
            // Left and right flank cover walls
            Bounds(8.0f, 6.0f, 10.0f, 14.0f),
            Bounds(30.0f, 6.0f, 32.0f, 14.0f),
            
            // Corner safety pillars
            Bounds(4.0f, 4.0f, 6.0f, 6.0f),
            Bounds(4.0f, 14.0f, 6.0f, 16.0f),
            Bounds(34.0f, 4.0f, 36.0f, 6.0f),
            Bounds(34.0f, 14.0f, 36.0f, 16.0f)
        };

        for (Bounds b : interiorLayout) {
            Obstacle o(*this, b);
            o.color = glm::vec3(0.2118, 0.5333, 0.5412); 
            obstacles.push_back(o);
            scene.addAsset(obstacles.back().mesh); // .back() safely gets the element you just pushed
        }
	}

	background.pos = glm::vec3(0,0,3);
	background.vertexComp(vector<unsigned int>{2});
	background.vertices(vector<float>{
		0,0,
		0,worldDim.y,
		worldDim.x,worldDim.y,
		worldDim.x,0
	});
	background.indices(vector<unsigned int>{
		0,1,2,
		0,2,3
	});

	
	scene.loadFunc = [&]() {
		background.updateBuffer();
		for (Obstacle& o : obstacles) o.mesh->updateBuffer();
		player1.mesh.updateBuffer();
		player2.mesh.updateBuffer();
	};


	drawTick.permanent.store(true);
	drawTick.preferedPriority.store(0);
	drawTick.f = [&]() { // MARK: Map Rendering
		if (glfwGetKey(render.window, GLFW_KEY_ESCAPE)) {
			render.running.store(false);
			return;
		};

		Render::Shader* s = shaders.at("rect");
		Render::Camera& cam = camera;

		glUseProgram(s->ID);
		glEnable(GL_DEPTH_TEST);
		// glDisable(GL_DEPTH_TEST);

		glm::mat4 viewMat = glm::mat4(1.0f);
		glm::mat4 screenMat = glm::orthoLH_NO(
			0.0f, 40.0f,
			0.0f, 20.0f,
			-1.0f, 100.0f
		);

		glUniformMatrix4fv(glGetUniformLocation(s->ID, "viewMat"), 1, GL_FALSE, glm::value_ptr(cam.getTransform())); 
		glUniformMatrix4fv(glGetUniformLocation(s->ID, "screenMat"), 1, GL_FALSE, glm::value_ptr(cam.getPerspective()));

		auto draw = [&](Render::Mesh* mesh, glm::vec3 color) {
			if (!mesh->isLoaded()) return;
			glUniformMatrix4fv(glGetUniformLocation(s->ID, "worldMat"), 1, GL_FALSE, glm::value_ptr(mesh->getTransform()));
			
			glUniform3fv(glGetUniformLocation(s->ID, "color"),1,glm::value_ptr(color));

			glBindVertexArray(mesh->VAO);
			glDrawElements(GL_TRIANGLES, mesh->totalIndices(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		};

		draw(&background,glm::vec3(0.5294, 0.8078, 0.9804));

		for (Obstacle& o : obstacles) {
			draw(o.mesh,o.color);
		}
		glUseProgram(0);
	};

	scene.addAsset(&background);
	scene.addAsset(&drawTick);

	render.setScene("scene1");
	respawnPlayers();
}
void Game::respawnPlayers() {
	glm::vec2 offset(5,10);
	player1.pos = offset;
	player2.pos = glm::vec2(worldDim.x - offset.x - 1,offset.y);
}

Game::Obstacle::Obstacle (Game& g, Bounds b) : game(g), bounds(b) {
	mesh = new Render::Mesh(g.render);
	mesh->pos = glm::vec3(b.p1,2);
	glm::vec2 dim = b.p2 - b.p1;
	
	mesh->vertexComp(vector<unsigned int>{2});
	mesh->vertices(vector<float>{
		0,0,
		0,dim.y,
		dim.x,dim.y,
		dim.x,0
	});
	mesh->indices(vector<unsigned int>{
		0,1,2,
		0,2,3
	});
}

Game::Bounds::Bounds (glm::vec2 P1, glm::vec2 P2) {
	p1.x = min(P1.x,P2.x);
	p1.y = min(P1.y,P2.y);

	p2.x = max(P1.x,P2.x);
	p2.y = max(P1.y,P2.y);
}
void Game::Bounds::genPoints (float dist) {
	checkPnts.clear();

	vector<array<glm::vec2,2>> sides = {
		{p1,glm::vec2(p1.x,p2.y)},
		{glm::vec2(p1.x,p2.y),p2},
		{p2,glm::vec2(p2.x,p1.y)},
		{glm::vec2(p2.x,p1.y),p1}
	};

	checkPnts.push_back(p1);
	checkPnts.push_back(p2);
	checkPnts.push_back(glm::vec2(p1.x,p2.y));
	checkPnts.push_back(glm::vec2(p2.x,p1.y));

	for (array<glm::vec2,2> s : sides) {
		glm::vec2 v1 = s.at(0);
		glm::vec2 v2 = s.at(1);

		float len = glm::distance(v1,v2);
		if (len < dist) continue;
		glm::vec2 v = glm::normalize(v2-v1) * dist;
		
		float d = 0;
		while (d < len/2) {
			d+=dist;
			if (d >= len/2) {
				checkPnts.push_back(v1 + v*(len/2));
				break;
			}
			glm::vec2 pl = v1 + v*d; //point coming from left
			glm::vec2 pr = v2 - v*d; //point coming from right

			checkPnts.push_back(pl);
			checkPnts.push_back(pr);
		}
	}
}
bool Game::Bounds::collides (Bounds b) {
	//p1 is bottom left, p2 is top right
	return (p1.x <= b.p2.x && p2.x >= b.p1.x && p1.y <= b.p2.y && p2.y >= b.p1.y);
}
void Game::Bounds::translate (glm::vec2 amount) {
	p1+=amount;
	p2+=amount;
}

Game::Player::Player (Game& g) : game(g), movement(game.render), drawTick(game.render), mesh(game.render) {
	dim = glm::vec2(1,1);

	mesh.vertexComp(vector<unsigned int>{2});
	mesh.vertices(vector<float>{
		0,0,
		0,dim.y,
		dim.x,dim.y,
		dim.x,0
	});
	mesh.indices(vector<unsigned int>{
		0,1,2,
		0,2,3
	});

	movement.preferedPriority.store(3);
	movement.permanent.store(true);
	movement.f = [&]() { // MARK: Player Movement
		if (!game.gameRunning) return;

		Bounds curBounds = getBounds();

		float gravity = -70;
		glm::vec2 acc(0,gravity * (onWall ? .3 : 1));

		int mvDir = (glfwGetKey(game.render.window, movementKeys.at(1)) ? -1 : 0) + (glfwGetKey(game.render.window, movementKeys.at(3)) ? 1 : 0);
		float trgtSpd = 12 * mvDir;
		float trgtDT = .1;
		if (trgtSpd == 0) {
			//friction
			float coef = (grounded ? 20 : 5) * (vel.x < 0 ? 1 : (vel.x > 0 ? -1 : 0));
			acc.x += coef; // 10 newton weight, arbitrary
		} else {
			if (vel.x == 0 || trgtSpd / vel.x < 0 || abs(vel.x) < abs(trgtSpd)) { //no vel, opposite directions or less than target
				acc.x += trgtSpd / trgtDT;
			}
		}

		float jumpAcc = -.4*gravity;
		if (glfwGetKey(game.render.window, movementKeys.at(0))) { // Up
			if (grounded && !usedJump) {
				vel.y = jumpAcc;
				usedJump = true;
			} else if (onWall && !usedWallJump) {
				vel = glm::normalize(glm::vec2(wallEjectSide,2)) * jumpAcc;
				usedWallJump = true;
			}
		}

		vel = vel + acc * (float)movement.dt();

		//Calculate collisions
		Bounds yTest(curBounds);
		yTest.translate(glm::vec2(0,vel.y * (float)movement.dt()));
		bool collidesY = false;
		grounded = false;
		for (Obstacle& o : game.obstacles) {
			if (o.bounds.collides(yTest)) { collidesY = true; break; }
		}
		if (collidesY) {
			if (vel.y < 0) grounded = true;
			vel.y = 0;
		}
		if (!grounded) usedJump = false;

		Bounds xTest(curBounds);
		xTest.translate(vel * (float)movement.dt());
		bool collidesX = false;
		for (Obstacle& o : game.obstacles) {
			if (o.bounds.collides(xTest)) { collidesX = true; break; }
		}
		if (collidesX) {
			onWall = true;
			wallEjectSide = (vel.x > 0 ? -1 : 1);

			vel.x = 0;
		} else if (vel.x != 0) onWall = false;
		if (!onWall) usedWallJump = false;

		pos+= vel * (float)movement.dt();
	};


	drawTick.preferedPriority.store(0);
	drawTick.permanent.store(true);
	drawTick.f = [&]() { // MARK: Player Rendering
		mesh.pos = glm::vec3(pos,0);

		Render::Shader* s = game.shaders.at("rect");
		Render::Camera& cam = game.camera;

		glUseProgram(s->ID);

		glEnable(GL_DEPTH_TEST);


		glUniformMatrix4fv(glGetUniformLocation(s->ID, "worldMat"), 1, GL_FALSE, glm::value_ptr(mesh.getTransform()));
		glUniformMatrix4fv(glGetUniformLocation(s->ID, "viewMat"), 1, GL_FALSE, glm::value_ptr(cam.getTransform())); 
		glUniformMatrix4fv(glGetUniformLocation(s->ID, "screenMat"), 1, GL_FALSE, glm::value_ptr(cam.getPerspective())); 
		
		glUniform3fv(glGetUniformLocation(s->ID, "color"),1,glm::value_ptr(color));

		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, mesh.totalIndices(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	};
	

	game.scene.addAsset(&movement);
	game.scene.addAsset(&drawTick);
	game.scene.addAsset(&mesh);
}
Game::Bounds Game::Player::getBounds () {
	Bounds b(glm::vec2(0,0),dim);
	b.translate(pos);
	return b;
}