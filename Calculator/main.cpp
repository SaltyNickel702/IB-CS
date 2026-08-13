#define DEBUG false

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <format>
#include <cctype>

using namespace std;

bool RUNNING = true;

// char newType = ((string("()").find(c) != string::npos) ? 'b' : ((string(".0123456789").find(c) != string::npos) ? 'f' : 'o'));
struct Term {
    Term (string Input) : value(NAN) {
        // remove whitespace
        for (char c : Input) {
            if (isspace(static_cast<unsigned char>(c))) continue;
            if (string("+-*/().0123456789").find(c) == string::npos) { //check allowed characters
                Error = format("The character \"{}\" was not recognized", string(1, c));
                return;
            }
            T.push_back(c);
        }


        // "-" is part of a number, not a subtraction operation
        // Therefore, all "-" that come after a number are actually a+-b, and should have the + before them
        for (int i = 1; i < T.length(); i++) {
            char c = T.at(i);
            char prev = T.at(i-1);
            int i2 = i;
            if (c == '-') {
                if (string(".1234567890)").find(prev) != string::npos) { //) signfies a term/number as well
                    //Insert + infront of -
                    T.insert(T.begin() + i, '+');
                    i++;
                } else if (prev == '-') {
                    //Double negatives get erased
                    T.erase(T.begin() + i-1, T.begin() + i + 1);
                    i--;
                }
            }
        }


        // Try separating by +
        int unresBrkt = 0; // Open brackets without closed (Unresolved) | Should never be -
        string crnt;

        for (char c : T) {
            if (unresBrkt == 0 && c == '+') { //only looking in outermost expression line
                if (crnt.length() > 0) {
                    nested.push_back(Term(crnt));
                    ops.push_back(c);
                    crnt = "";
                } else {
                    if (T.at(0) == c) {
                        Error = "Expressions and Terms Cannot Start with a +";
                        return;
                    }
                    Error = "Expressions cannot have multple + in a row";
                    return;
                }
            } else {
                if (c == '(') unresBrkt++;
                if (c == ')') {
                    if (unresBrkt == 0) {
                        Error = "Closing brackets must have a matching open bracket";
                        return;
                    }
                    unresBrkt--;
                }
                crnt.push_back(c);
            }
        }
        if (nested.size() != 0) {
            if (crnt.length() == 0) {
                Error = "Expressions cannot end with a +";
                return;
            }
            nested.push_back(Term(crnt));
            goto EvaluateExpression; //Multiple terms was found
        }
        
        
        //Try same thing but for *,/
        unresBrkt = 0;
        crnt = "";

        for (char c : T) {
            if (unresBrkt == 0 && (c == '*' || c == '/')) { //only looking in outermost expression line
                if (crnt.length() > 0) {
                    nested.push_back(Term(crnt));
                    ops.push_back(c);
                    crnt = "";
                } else {
                    if (T.at(0) == c) {
                        Error = format("Expressions and Terms Cannot Start with a {}",c);
                        return;
                    }
                    Error = "Expressions cannot have multple * or / in a row";
                    return;
                }
            } else {
                if (c == '(') unresBrkt++;
                if (c == ')') {
                    if (unresBrkt == 0) {
                        Error = "Closing brackets must have a matching open bracket";
                        return;
                    }
                    unresBrkt--;
                }
                crnt.push_back(c);
            }
        }
        if (nested.size() != 0) {
            if (crnt.length() == 0) {
                Error = "Expressions cannot end with a * or /";
                return;
            }
            nested.push_back(Term(crnt));
            goto EvaluateExpression; //Multiple terms was found
        }

        //Check for Brackets
        unresBrkt = 0;
        crnt = "";

        for (char c : T) {
            if (unresBrkt == 0 && c == '(') { // only looking in outermost expression line
                if (crnt.length() > 0) {
                    nested.push_back(Term(crnt));
                    ops.push_back('*'); // numbers are multiplied when next to brackets
                }
                crnt = "";
                unresBrkt++;
            } else {
                if (c == '(') unresBrkt++;
                if (c == ')') {
                    if (unresBrkt == 0) {
                        Error = "Closing brackets must have a matching open bracket";
                        return;
                    }
                    unresBrkt--;

                    if (unresBrkt == 0) { // All brackets now closed, term is done
                        nested.push_back(Term(crnt));
                        ops.push_back('*'); // Either the last term, or another one. No such thing as too many ig
                        crnt = "";
                    } else {
                        crnt.push_back(c);
                    }
                } else {
                    crnt.push_back(c);
                }
            }
        }
        //if 
        if (unresBrkt != 0) { // this is where we check for 
            Error = "There must be the same about of open and closing brackets";
            return;
        }


    EvaluateExpression:
        #if DEBUG
            cout << "Term:  " << T << "\tDiv: ";
            for (Term t : nested) cout << t.T << " ";
            // for (char o : ops) cout << o << endl;
            cout << endl;
        #endif

        if (nested.size() == 0) {
            // Single Number
            // Do checks to make sure number is valid
            try {
                value = stof(T);
            } catch (const std::invalid_argument& e) {
                Error = format("{} is not a valid number",T);
                return;
            } catch (const std::out_of_range& e) {
                Error = format("{} is out of range of a float",T);
                return;
            }
            return;
        }
        if (nested.size() == 1) {
            //Single Bracket Set
            value = nested.at(0).value;
            Error = nested.at(0).Error;
            return;
        }
        
        if (nested.size() - ops.size() < 1) {
            Error = format("There is a mismatch between terms and expressions in {}, with {} terms detected and {} operators",T,nested.size(),ops.size());
            return;
        }

        value = 0;
        for (int i = 0; i < nested.size(); i++) {
            Term n = nested.at(i);
            if (n.Error != "") {
                Error = n.Error;
                return;
            }
            if (i == 0) {
                value = n.value;
                continue;
            }
            char o = ops.at(i-1);
            switch (o) {
                case '+':
                    value+= n.value;
                    break;
                case '*':
                    value*= n.value;
                    break;
                case '/':
                    value/= n.value;
                    break;
            }
        }
    };
    
    string T;
    float value;
    string Error;

    private:
        vector<Term> nested;
        vector<char> ops;
};

void runCalc () {
    cout << ": ";
    string S;
    getline(cin,S);
    if (S.length() == 0) return;
    if (S == "quit" || S == "exit") {RUNNING = false; return;}
    Term T(S);
    if (!(T.Error == "")) {
        cout << "Error: " << T.Error << endl;
        return;
    }
    cout << "Answer: " << T.value << endl;
}

int main () {
    while (RUNNING) {
        runCalc();
    }
    return 0;
}