#include "Render.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <format>
#include <chrono>
#include <ctime>
#include <glm/gtc/matrix_transform.hpp>


using namespace std;

#pragma region Asset
Render::Asset::Asset (Render& r) : render(r) {};
#pragma endregion


#pragma region Shader
Render::Shader::Shader (string v, string f, Render& r) : Asset(r) {
	ID = 0;
	vertexPath = v;
	fragmentPath = f;
}
void Render::Shader::load () {
	string vertexCode;
	string fragmentCode;

	ifstream vShader;
	ifstream fShader;

	vShader.exceptions(ifstream::failbit | ifstream::badbit);
	fShader.exceptions(ifstream::failbit | ifstream::badbit);

	//Read Files
	try {
		vShader.open("./assets/shaders/" + vertexPath);
		fShader.open("./assets/shaders/" + fragmentPath);

		stringstream vShaderStream, fShaderStream;
		vShaderStream << vShader.rdbuf();
		fShaderStream << fShader.rdbuf();

		vShader.close();
		fShader.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	} catch (ifstream::failure e) {
		cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << endl;
	}
	const char* vertexCString = vertexCode.c_str();
	const char* fragmentCString = fragmentCode.c_str();


	//Compile Shaders
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	//Vertex
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex,1,&vertexCString,NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex,GL_COMPILE_STATUS,&success);
	if (!success) {
		glGetShaderInfoLog(vertex,512,NULL,infoLog);
		cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}

	//fragment
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment,1,&fragmentCString,NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment,GL_COMPILE_STATUS,&success);
	if (!success) {
		glGetShaderInfoLog(fragment,512,NULL,infoLog);
		cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
	}

	
	//Program Creation
	ID = glCreateProgram();
	glAttachShader(ID,vertex);
	glAttachShader(ID,fragment);
	glLinkProgram(ID);

	glGetProgramiv(ID,GL_LINK_STATUS,&success);
	if (!success) {
		glGetShaderInfoLog(fragment,512,NULL,infoLog);
		cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
void Render::Shader::unload () {
	glDeleteProgram(ID);
	ID = 0;
}
#pragma endregion


#pragma region Texture
Render::Texture::Texture (Render& r) : Asset(r) {

}
#pragma endregion


#pragma region Mesh
Render::Mesh::Mesh (Render& r) : Asset(r) {
	VAO = 0;
	VBO = 0;
	EBO = 0;

	vertL = 0;
	indL = 0;
	attrL = 0;

	scale = glm::vec3(1);
	pos = glm::vec3(0);
	rot = glm::quat(1, 0, 0, 0);
}
void Render::Mesh::updateBuffer() {
	TickFunc* t = new TickFunc(render);
	t->deleteAfter.store(true);
	t->f = [&]() {
		bool newBuffer = !loaded.load();
		if (newBuffer) {
			cleanData();

			glGenVertexArrays(1,&VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			loaded.store(true);
		}

		lock_guard<mutex>* lock = new lock_guard<mutex>(vectorMutex);

		bool vertLizeChanged = (vert == nullptr || vertL != vertV.size());
		bool indLizeChanged = (ind == nullptr || indL != indV.size());
		bool attrChanged = (attr == nullptr || attrL != attrV.size());
		int oldattrL = attrL;
		if (!attrChanged) {
			// Same size as before, now check if composition changed
			for (int i = 0; i < attrV.size(); i++) {
				if (attrV.at(i) != attr[i]) {
					attrChanged = true;
					break;
				}
			}
		}

		vert = vertV.data();
		ind = indV.data();
		attr = attrV.data();	
		
		vertL = vertV.size();
		indL = indV.size();
		attrL = attrV.size();

		delete lock;

		glBindVertexArray(VAO);
		GLenum drawT = drawType.load();

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		if (vertLizeChanged || newBuffer) { glBufferData(GL_ARRAY_BUFFER, vertL * sizeof(float), vert, drawT);
		} else glBufferSubData(GL_ARRAY_BUFFER, 0, vertL * sizeof(float), vert);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		if (vertLizeChanged || newBuffer) { glBufferData(GL_ELEMENT_ARRAY_BUFFER, indL * sizeof(unsigned int), ind, drawT);
		} else glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indL * sizeof(unsigned int), ind);

		// Calculate attribute layout
		attrPerVert = 0;
		vector<unsigned int> sums(attrL); //Stride per vertex
		for (int i = 0; i < attrL; i++) {
			sums[i] = attrPerVert;
			attrPerVert += attr[i];
		}
		totalVert = vertL / attrPerVert;

		if (attrChanged) for (int i = 0; i < oldattrL; i++) glDisableVertexAttribArray(i); // disable all attributes, reenable used later
		if (attrChanged || newBuffer) {
			for (int i = 0; i < attrL; i++) {
				glVertexAttribPointer(i, attr[i], GL_FLOAT, GL_FALSE, attrPerVert * sizeof(float), (void*)(sums[i] * sizeof(float)));
				glEnableVertexAttribArray(i);
			}
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	};
	t->push(-1);
}
void Render::Mesh::cleanData() {
	loaded.store(false);

	if (glIsVertexArray(VAO)) glDeleteVertexArrays(1, &VAO);
    if (glIsBuffer(VBO)) glDeleteBuffers(1, &VBO);
    if (glIsBuffer(EBO)) glDeleteBuffers(1, &EBO);

	VAO = 0;
	VBO = 0;
	EBO = 0;
}
vector<float> Render::Mesh::vertices() {
	lock_guard<mutex> lock(vectorMutex);
	return vertV;
}
vector<unsigned int> Render::Mesh::indices() {
	lock_guard<mutex> lock(vectorMutex);
	return indV;
}
vector<unsigned int> Render::Mesh::vertexComp() {
	lock_guard<mutex> lock(vectorMutex);
	return attrV;
}
void Render::Mesh::vertices(vector<float> v) {
	lock_guard<mutex> lock(vectorMutex);
	vertV = v;
}
void Render::Mesh::indices(vector<unsigned int> v) {
	lock_guard<mutex> lock(vectorMutex);
	indV = v;
}
void Render::Mesh::vertexComp(vector<unsigned int> v) {
	lock_guard<mutex> lock(vectorMutex);
	attrV = v;
}

glm::mat4 Render::Mesh::getTransform() {
	glm::mat4 transl = glm::translate(glm::mat4(1.0f), pos);
	glm::mat4 rotMat = glm::mat4_cast(rot);
	// glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f),scale);

	return transl * rotMat;// * scaleMat;
}
#pragma endregion


#pragma region TickFunc
Render::TickFunc::TickFunc (Render& r) : Asset(r) {
	
}
void Render::TickFunc::push (int priority) {
	curPriority = (priority < -1) ? 0 : priority;
	{
		lock_guard<mutex> lock(render.addTickMutex);
		render.addToTick.push_back(this);
	}
}
void Render::TickFunc::pull () {
	{
		lock_guard<mutex> lock(render.removeTickMutex);
		render.removeFromTick.push_back(this);
	}
}
#pragma endregion


#pragma region Camera
Render::Camera::Camera () {
	rot = glm::quatLookAtLH(glm::vec3(0,0,1),glm::vec3(0,1,0));
	pos = glm::vec3(0);
}

glm::mat4 Render::Camera::getTransform() {
	glm::mat4 viewRot = glm::mat4_cast(glm::conjugate(rot));
	glm::mat4 viewTransl = glm::translate(glm::mat4(1.0f), -pos);

	return viewRot * viewTransl;
}
glm::mat4 Render::Camera::getPerspective () {
	return glm::mat4(1.0f);
}

glm::mat4 Render::Camera::Perspective::getPerspective () {
	return glm::perspectiveLH_NO(glm::radians(fov), aspectRatio, near, far);
}
glm::mat4 Render::Camera::Orthographic::getPerspective () {
	return glm::orthoLH_NO(left,right,bottom,top,near,far);
}
#pragma endregion


#pragma region Scene
Render::Scene::Scene (string id, Render& r) : render(r), ID(id) {
	if (r.scenes.contains(ID)) throw invalid_argument(format("{} is an existing scene.",ID));
	r.scenes.insert({ID,this});
}
void Render::Scene::loadScene () {
	TickFunc* t = new TickFunc(render);
	t->deleteAfter.store(true);
	t->f = [&]() {
		for (Asset* a : assets) {
		a->load();
		}
		loadFunc();
	};
	t->push(-1);
}
void Render::Scene::unloadScene () {
	TickFunc* t = new TickFunc(render);
	t->deleteAfter.store(true);
	t->f = [&]() {
		for (Asset* a : assets) {
			a->unload();
		}	
	};
	t->push(-1);
}
void Render::Scene::addAsset (Asset* a) {
	auto it = find(assets.begin(), assets.end(), a);
	if (it == assets.end()) assets.push_back(a);
}
void Render::Scene::removeAsset (Asset* a) {
	a->unload();
	auto it = find(assets.begin(), assets.end(), a);
	if (it != assets.end()) assets.erase(it);
}
void Render::setScene (string ID) {
	if (!scenes.contains(ID)) {
		cerr << format("{} is not a valid scene ID", ID); // No error thrown because not a critical error
		return;
	}

	if (!curScene.empty() && scenes.contains(curScene)) {
        scenes[curScene]->unloadScene();
    }
	curScene = ID;
	scenes.at(ID)->loadScene();
}
void Render::setScene (Scene* s) {
	if (&s->render != this) {
		cerr << format("Scene \"{}\" is not part of a different Render instance",s->ID) << endl;
		return;
	}
	setScene(s->ID);
}
#pragma endregion


#pragma region Render Functions
Render::Render (int w, int h, string t) : title(t) {
	dim = glm::vec2(w,h);

	Thread = new thread(&Render::init,this);
}
void Render::init () {
	// Initialize GLFW
	if (!glfwInit()) {
		throw runtime_error("Failed to initialize GLFW");
	}

	// Configure OpenGL version 3.3 Core Profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DEPTH_BITS, 24); 

	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	// Create window and context
	window = glfwCreateWindow(dim.x, dim.y, title.c_str(), NULL, NULL);
	if (!window) {
		glfwTerminate();
		throw runtime_error("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw runtime_error("Failed to initialize GLAD");
	}

	running.store(true);

	loop();
}
void Render::loop () {
	while (running.load() && !glfwWindowShouldClose(window)) {
		//Lock and copy queues
		vector<TickFunc*> removeCopy;
		vector<TickFunc*> addCopy;
		{
			lock_guard<mutex> removeLock(removeTickMutex);
			lock_guard<mutex> addLock(addTickMutex);

			removeCopy = removeFromTick;
			removeFromTick.clear();
			addCopy = addToTick;
			addToTick.clear();
		}

		// Pull requested TickFunc
		for (TickFunc* T : removeCopy) {
			auto it = find(tickFunctions.begin(), tickFunctions.end(), T);
			if (it == tickFunctions.end()) continue; // Not currently added

			tickFunctions.erase(it);
		}

		// Push TickFunc onto the tick queue
		for (TickFunc* T : addCopy) {
			auto it = find(tickFunctions.begin(), tickFunctions.end(), T);
			if (it != tickFunctions.end()) continue; //already added, needs to removed first
			
			auto loc = lower_bound(tickFunctions.begin(), tickFunctions.end(), T, [](const TickFunc* t1, const TickFunc* t2) { 
				int p1 = t1->getPriority();
				int p2 = t2->getPriority();

				if (p1 == -1 && p2 != -1) return true;
				if (p2 == -1) return false;
				
				return p1 > p2;
			});
			tickFunctions.insert(loc,T);
		}

		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwPollEvents();

		//Calc dT
		auto updateTime = [&]() {
			currentTime = glfwGetTime();
			deltaTick = currentTime - lastTime;
		};
		updateTime();
		// if (enableFrameLimiter.load() && fps() > frameLimit.load()) { // Might need to update this to be more precise
		// 	double targetDT = 1.0 / frameLimit.load();
		// 	double difMS = (targetDT - deltaTick) * 1000000;
		// 	this_thread::sleep_for(chrono::microseconds(static_cast<int>(difMS)));
		// 	updateTime();
		// }

		//Run all TickFunc
		for (TickFunc* t : tickFunctions) {
			if (t->f) t->f();
		}

		//Remove single run TickFunc
		vector<TickFunc*> toDelete;
		erase_if(tickFunctions,[&toDelete](TickFunc* t) {
			bool p = t->permanent.load();
			if (!p && t->deleteAfter.load()) toDelete.push_back(t);
			return !p;
		});
		for (TickFunc* t : toDelete) delete t;

		glfwSwapBuffers(window); //updates screen buffer

		lastTime = currentTime;
	}
}
#pragma endregion
