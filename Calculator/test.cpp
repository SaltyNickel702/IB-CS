#include <iostream>
#include <format>

using namespace std;

int main () {
	cout << stof("-.5") << endl;
	cout << format("{} is my number", 5) << endl;
	return 0;
}