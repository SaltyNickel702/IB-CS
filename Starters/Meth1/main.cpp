#include <iostream>
#include <vector>
#include <algorithm>

int middleValue (int i1, int i2, int i3) {
    std::vector<int> v{i1,i2,i3};
    std::sort(v.begin(), v.end());
    return v.at(1);
}

int main () {
    std::cout << middleValue(2, 9, 5) << std::endl;
    std::cout << middleValue(12, 4, 8) << std::endl;
    std::cout << middleValue(7, 3, 10) << std::endl;
}