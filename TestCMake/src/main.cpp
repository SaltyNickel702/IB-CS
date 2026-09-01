#include <iostream>
#include "UI.h"

using namespace std;

int main () {
	UI u(100,20);
	u.fillChar = '.';
	u.display();

	return 0;
}