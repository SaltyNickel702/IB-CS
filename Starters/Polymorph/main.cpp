#include <iostream>
#include <format>
#include <glm/glm.hpp> // Requires GLM package installed
#include <vector>
#include <ctime>

using namespace std;

struct Character {
	Character () { mvSpd = 3; };
	float mvSpd;

	void move(glm::vec2 dir) {
		pos+= glm::normalize(dir) * mvSpd;
		pos.x = round(pos.x);
		pos.y = round(pos.y);
	}
	glm::vec2 getPos () { return pos; };
	virtual void display () {
		cout << format(
R"(
@ ({},{}):
|/
^
)",pos.x,pos.y) << endl;
	}

	private:
		glm::vec2 pos;
};
struct Ogre : public Character {
	Ogre () { mvSpd = 1; };

	void display () override {
		glm::vec2 p = getPos();
		cout << format(
R"(
@ ({},{}):
/##\
 ##
)",p.x,p.y) << endl;
	};
};
struct Mouse : public Character {
	Mouse () { mvSpd = 5; };

	void display () override {
		glm::vec2 p = getPos();
		cout << format(
R"(
@ ({},{}):
-=>
)",p.x,p.y) << endl;
	};
};

int main () {
	vector<Character*> characters {
		new Character(),
		new Ogre(),
		new Mouse()
	};

	srand(time(0));
	for (Character* c : characters) {
		glm::vec2 dir((float)rand(),(float)rand());
		dir /= (float)RAND_MAX;
		c->move(dir);
		c->display();
	};
	return 0;
}