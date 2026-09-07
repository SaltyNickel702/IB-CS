/*

Notes:
All functions are designed to be able to be called from outside the render loop.
To run any code inside the render loop, use a TickFunc struct. Set to permanent to true to run function every frame

*/


#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <thread>
#include <functional>
#include <atomic>
#include <mutex>
#include <map>


struct Render { // OpenGL window instance
	// MARK: Asset
	struct Asset { // Asset Superclass, holds structure for loading and unloading all assets in a scene
		Asset (Render& render);
		Render& render;

		// bool loaded; // Not sure if this is needed
		virtual void load() {};
		virtual void unload() {}; // unload data w/ out deleting Asset instance, allowing for reloading | Must be called on the Render-loop

		~Asset () { }; // I need to edit this to make sure variables are deleted on the openGL thread
	};
	
	// MARK: Shader
	struct Shader : public Asset { // Holds Shader data for GPU programs
		unsigned int ID;

		Shader (std::string vertexPath, std::string fragmentPath, Render& render);

		void load() override;
		void unload() override;

		private:
			std::string vertexPath;
			std::string fragmentPath;
	};
	
	// MARK: Texture
	struct Texture : public Asset { // Holds texture for use by shaders
		Texture (Render& r);
	};

	// MARK: Mesh
	struct Mesh : public Asset { // Holds and manages vertex data. Is ONLY UNLOADED by scene manager. Rendering is managed by user
		Mesh(Render& render);
		struct Primitives;

		glm::vec3 pos;
		glm::quat rot;
		glm::vec3 scale;

		glm::mat4 getTransform (); // Use in shader to translate relative vertices to world space

		std::vector<float> vertices();
		std::vector<unsigned int> indices();
		std::vector<unsigned int> vertexComp();
		void vertices(std::vector<float> vertices);
		void indices(std::vector<unsigned int> indices);
		void vertexComp(std::vector<unsigned int> vertexComp);

		int totalIndices () const { return indL; };

		void updateBuffer();
		void cleanData();
		void unload () override { cleanData(); };
		bool isLoaded () const { return loaded.load(); };

		unsigned int VAO, EBO, VBO;
		std::atomic<GLenum> drawType = GL_STATIC_DRAW;

		private:
			float* vert;
			unsigned int* ind;
			unsigned int* attr;
			int attrPerVert;
			int totalVert;

			int vertL; // lengths of arrays
			int indL;
			int attrL;

			std::mutex vectorMutex;
			std::vector<float> vertV; // Only convert to pointer arrays during buffering
			std::vector<unsigned int> indV;
			std::vector<unsigned int> attrV;

			std::atomic<bool> loaded = false; // Do the buffers exist

	};
	struct Mesh::Primitives { // Primitive Mesh types
		Primitives () = delete;
		struct Sphere : public Mesh {
			Sphere ();
		};
	};
	
	//MARK: TickFunc
	struct TickFunc : public Asset { // Wrapper for running a function on the Render loop
		TickFunc(Render& r);
		
		void push (int priority); // Push into next frame's tick queue
		void pull (); // Pull TickFunc from TickFunctions

		std::atomic<bool> permanent = false;
		std::atomic<bool> deleteAfter = false;
		std::atomic<int> preferedPriority = 0;
		int getPriority () const { return curPriority; };

		double dt() const { return render.getDeltaTick(); }; // Shorthands
		double fps() const { return render.fps(); };

		std::function<void()> f = [](){}; // Encapsulated Function

		void load() override { push(preferedPriority.load()); };
		void unload() override { pull(); };

		private:
			int curPriority;
	};

	// MARK: Camera
	struct Camera {
		Camera();

		glm::vec3 pos;
		glm::quat rot;

		virtual glm::mat4 getTransform (); // Returns world transform
		virtual glm::mat4 getPerspective (); // Returns perspective warp to screen space

		struct Perspective;
		struct Orthographic;
	};
	struct Camera::Perspective : public Camera {
		float fov;
		float near, far;
		float aspectRatio;

		glm::mat4 getPerspective () override;
	};
	struct Camera::Orthographic : public Camera {
		float left, right;
		float top, bottom;
		float near, far;

		glm::mat4 getPerspective () override;
	};


	// MARK: Scene
	struct Scene { // Scene is a container for assets to manage the loading into opengl
		Scene (std::string ID, Render& render);
		
		Render& render;
		const std::string ID;

		void addAsset(Asset* asset);
		void removeAsset(Asset* asset);

		void loadScene();
		void unloadScene();
		std::function<void()> loadFunc = [](){}; // Any additional code that must execute before scene loading
		

		private:
			std::vector<Asset*> assets; 
	};
	std::map<std::string, Scene*> scenes;
	void setScene (std::string sceneName);
	void setScene (Scene* scene);
	std::string getScene () const { return curScene; }; // Returns current scene ID

	// MARK: Render Misc.
	Render (int width, int height, std::string title); // Initializes new OpenGL rendering window

	GLFWwindow* window;
	std::thread* Thread;
	glm::vec2 dim;
	const std::string title;
	std::atomic<bool> running;

	std::atomic<bool> enableFrameLimiter = true;
	std::atomic<unsigned int> frameLimit = 60;
	double getDeltaTick() const { return deltaTick; }; // dT is read only
	double fps() const { return 1.0 / getDeltaTick(); };


	private:
		double deltaTick;
		double lastTime = 0;
		double currentTime;

		std::mutex addTickMutex; // Creating queues allows there to be smaller chance of mutex being locked when adding TickFunc
		std::mutex removeTickMutex;
		std::vector<TickFunc*> tickFunctions;
		std::vector<TickFunc*> addToTick;
		std::vector<TickFunc*> removeFromTick;

		std::string curScene;

		void init();
		void loop();
};