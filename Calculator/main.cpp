#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

bool RUNNING = true;

struct parser { //terms will be auto ordered
    parser (string S) : error("") {
        
    };
    vector<float> terms;
    vector<char> opps;
    string error;

    float calc () {
        if (terms.size() == 0) return NAN;
        float n = terms.at(0);
        for (int i = 1; i <= opps.size(); i++) {
            char o = opps.at(i-1);
            switch (o) {
                case '+':
                    break;
                case '-':
                    break;
                case '*':
                    break;
                case '/':
                    break;
                case '^':
                    break;
            }
        }

        return n;
    };
};

void runCalc () {
    string S;
    getline(cin,S);
    if (S.length() == 0) return;
    parser P(S);
    if (!(P.error == "")) {
        cout << P.error << endl;
        return;
    }
}

int main () {
    while (RUNNING) {
        runCalc();
    }
    return 0;
}