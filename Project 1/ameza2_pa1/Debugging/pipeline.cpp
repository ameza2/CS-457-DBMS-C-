// Abraham Meza //
// CS 457 - Project 1 (DEBUGGER) //
// September 29, 2021 //

// Debugging Program: This program checks the pipleline file input, removing '/n' and '/r' characters from each string to illustrate line-by-line inputs. //

// Header File(s) //

#include <iostream>

// Linux Header File(s) //

#include <algorithm>

// Namespace Declaration(s) //

using namespace std;

// Main Program //

int main() {

    int found = -1; // helper-variable: used to record the position of a substring from str.find(); solution to prevent different signedness comparison issues (better than type-casting)

    char delimiter(';'); // delimiter variable: used for getline() delimiter (stop reading when reach ";")

    string input; // variable: used to take user-input database command input

    cout << "\n" <<
            "///////////////////////////////\n" <<
            "/////  Pipelining Program /////\n" <<
            "///////////////////////////////" << endl << endl; // test label design */

    while(found == -1) { // while loop: while ".EXIT" is not in the string, continue to take in user-input.

        getline(cin, input, delimiter); // getline(): retrieve user-input until it sees ';' as a delimiter

        cin.ignore(); // ignore new-line character input: clear cin buffer (prevents accidental new line from being seen as the keyword (first word))
        input.erase(remove(input.begin(), input.end(), '\n'), input.end()); // str.erase(): remove all new lines from user-input string (prevents '/n' in commands)
        input.erase(remove(input.begin(), input.end(), '\r'), input.end()); // str.erase(): remove all return characters from user-input string (prevents '/r' in commands): solves issue in where empty lines inside files were treated as command characters

        cout << "------------" << endl << ">> " << input << endl; // cout confirmation

        found = input.find(".EXIT"); // ".EXIT" does not use ';' as a delimiter; therefore, program needs to parse ".EXIT" to determine if the program should end (discard anything that follows ".EXIT")
    }

    return 0;
}