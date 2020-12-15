// Author: Luke Soguero
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

vector<string> S; // input set
int D; // max distance
vector<char> Alphabet{'a', 'c', 't', 'g'};

int hammingDistance(string s1, string s2) {
    if (s1.length() != s2.length()) {
        return -1;
    }
    int numDiff = 0;
    for (int i = 0; i < s1.length(); i++) {
        if (s1[i] != s2[i]) {
            numDiff++;
        }
    }
    return numDiff;
}

string closestString(string s, int d) {
    if (d < 0) {
        return "not found";
    }
    // check if we have moved candidate too far or if we have solved problem
    bool solved = true;
    string compare;
    for (int i = 0; i < S.size(); i++) {
        int dist = hammingDistance(s, S[i]);
        if (dist > D + d) {
            return "not found";
        }
        if (dist > D) {
            compare = S[i];
            solved = false;
        }
    }
    if (solved) {
        return s;
    }
    // find all mismatches between s and compare
    vector<int> mismatches;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] != compare[i]) {
            mismatches.push_back(i);
        }
    }
    // choose subset of mismatches with size D + 1
    mismatches.resize(D+1);
    for (int x : mismatches) {
        string newCandidate = s;
        newCandidate[x] = compare[x]; // make single character replacement and call recursive procedure on new candidate
        string sol = closestString(newCandidate, d-1);
        if (sol != "not found") {
            return sol;
        }
    }
    return "not found";
}

// fill global vector S with k strings of given length
void randomizeInput(int k, int length) {
    S.clear();
    // create random inital string
    string initial = "";
    for (int i = 0; i < length; i++) {
        int index = rand() % Alphabet.size(); // random index from 0 to 3
        char next = Alphabet[index];
        initial += next;
    }
    // create k strings that differ from initial string in D positions
    for (int i = 0; i < k; i++) {
        string s = initial;
        vector<int> mismatches;
        // choose mismatch positions
        while (mismatches.size() < D) {
            int m = rand() % length;
            // add mismatch to list if it is not already in it
            if (find(mismatches.begin(), mismatches.end(), m) == mismatches.end()) {
                mismatches.push_back(m);
            }
        }
        // make replacements
        for (int m : mismatches) {
            int index = rand() % Alphabet.size();
            char replacement = Alphabet[index];
            s[m] = replacement;
        }
        S.push_back(s);
    }
}

// finds the string in S which has the lowest avergae distance to all other strings in S
string getMinDistString() {
    vector<int> distances;
    for (int i = 0; i < S.size(); i++) {
        int totalDistance = 0;
        for (int j = 0; j < S.size(); j++) {
            if (j == i) continue;
            totalDistance += hammingDistance(S[i], S[j]);
        }
        distances.push_back(totalDistance);
    }
    // return string with min total distance
    int minIndex = 0;
    int minDist = 1000;
    for (int i = 0; i < distances.size(); i++) {
        if (distances[i] < minDist) {
            minDist = distances[i];
            minIndex = i;
        }
    }
    return S[minIndex];
}

int main() {
    ofstream output;
    output.open("output.csv");
    output << "d,k,time" << endl;
    // run algorithm on randomized instances of various sizes, with various D values
    vector<int> dVals{5, 10, 15, 20};
    int yes = 0;
    int total = 0;
    for (int val : dVals) {
        D = val;
        for (int i = 5; i <= 25; i++) {
            cout << D << ", " << i << endl;
            // take average of 10 trials
            int micros = 0;
            for (int j = 0; j < 10; j++) {
                randomizeInput(i, 50);
                auto start = high_resolution_clock::now();
                string initial = getMinDistString();
                string solution = closestString(initial, D);
                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<microseconds>(stop - start);
                micros += duration.count();
                if (solution != "not found") {
                    yes++;
                }
                total++;
            }
            double avg = micros/10.0;
            output << D << "," << i << "," << avg << "\n";
        }
    }
    //cout << (double)yes/total << endl;
    output.close();
    return 0;
}