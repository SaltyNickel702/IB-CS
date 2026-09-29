#include <iostream>
#include <vector>

using namespace std;

int main () {
	enum Color {
		red, blue, green // 0, 1, 2
	};

	vector<Color> A{red, blue};
	vector<Color> B = A;

	B.push_back(Color::green);

	for (Color c : A) cout << c << " ";
	cout << endl;
	for (Color c : B) cout << c << " ";

	return 0;
}