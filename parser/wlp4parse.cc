#include <iostream>
#include <bits/stdc++.h>
#include "wlp4data.h"
#include <sstream>

class State {

    int id;
    std::map<std::string, int> transitions;
    std::map<int, std::set<std::string>> reductions;

    public:

    State() {}

    ~State() {}

    State(int id, std::pair<std::string, int> transition) {
        this->id = id;
        this->transitions.insert(transition);
    }

    void addTransition(std::pair<std::string, int> transition) {
        transitions.insert(transition);
    }

    void addReduction(int derivationIdx, std::string reduction) {
        if (reductions.count(derivationIdx) == 0) {
            std::set<std::string> reduceSet;
            reduceSet.insert(reduction);
            reductions.insert({derivationIdx, reduceSet});
        } else {
            reductions.at(derivationIdx).insert(reduction);
        }
    }

    bool hasTransition(std::string symbol) {
        return transitions.count(symbol) > 0;
    }

    int getNextState(std::string symbol) {
        return transitions.at(symbol);
    }

    int needReduce(std::string next, int numDerivations) {

        for (int i = 0; i < numDerivations; i++) {
            if (reductions.count(i) == 0) continue;
            if (reductions.at(i).count(next) > 0) return i;
        }

        return -1;

    }

    bool checkAccept() {
        for (auto it = reductions.begin(); it != reductions.end(); ++it) {
            if (it->second.count(".ACCEPT") > 0) {
                return true;
            }
        }
        return false;
    }
};

class Node {

    std::string type;
    std::string lexeme;
    std::string prodRule;

    public:
    Node() {}
    ~Node() {}

    std::deque<Node> children;

    std::string getType() {
        return type;
    }

    std::string getLexeme() {
        return lexeme;
    }

    std::string getProdRule() {
        return prodRule;
    }

    void setType(std::string &type) {
        this->type = type;
    }

    void setLexeme(std::string &lexeme) {
        this->lexeme = lexeme;
    }

    void setProdRule(std::string &prodRule) {
        this->prodRule = prodRule;
    }

    void addChild(Node &child) {
        children.push_back(child);
    }

    bool isLeaf() {
        return children.empty();
    }

    Node(std::string &type, std::string &lexeme) {
        this->type = type;
        this->lexeme = lexeme;
    }
    
    Node(std::string &prodRule) {
        this->prodRule = prodRule;
    }

};

std::pair<std::string,std::string> splitByFirstSpace(std::string &line) {
    int spaceIdx = line.find(' ');
    std::string first = line.substr(0, spaceIdx);
    std::string second = "";
    if (spaceIdx < line.size()) {
        second = line.substr(spaceIdx + 1);
    }
    return std::make_pair(first,second);
}

std::vector<std::string> split(std::string &line) {

    int pos_start = 0;
    int pos_end = 1;

    std::string token;
    std::vector<std::string> arr;

    while ((pos_end = line.find_first_of(" \n\t", pos_start)) != std::string::npos) {
        int dist = pos_end - pos_start;
        token = line.substr(pos_start, dist);
        pos_start = pos_end + 1;
        arr.emplace_back(token);
    }

    arr.emplace_back(line.substr(pos_start));
    return arr;
}

void shift(std::deque<std::pair<std::string,std::string>> &inputSequence, std::deque<Node> &treeDeque, std::vector<std::pair<std::string, std::string>> &derivations, int &k) {
    auto firstElement = inputSequence[0];
    treeDeque.push_back(Node(firstElement.first, firstElement.second));
    if (firstElement.first != "BOF" && firstElement.first != "EOF") {
        k++;
        // std::cout << firstElement.first << " ";
    }
    
    inputSequence.pop_front();
}

int reduce(int reduceNum, std::vector<std::pair<std::string, std::string>> &derivations, std::deque<Node> &treeDeque, std::deque<int> &stateDeque) {

    int currState = stateDeque.back();

    std::pair<std::string, std::string> derivation = derivations[reduceNum];

    std::string prodRule = derivation.first + " " + derivation.second;

    Node newParent = Node(prodRule);

    newParent.setType(derivation.first);

    if (derivation.second != ".EMPTY") {

        std::vector<std::string> rhsDerivation = split(derivation.second);
        int i2 = treeDeque.size() - 1;
        
        for (int i = rhsDerivation.size() - 1; i >= 0; i--) {
            if (treeDeque[i2].getType() == rhsDerivation[i]) {

                newParent.addChild(treeDeque[i2]);
                i2--;
                treeDeque.pop_back();
                stateDeque.pop_back();

            } else {
                break;
            }
        }
        
        currState = stateDeque.back();

    } else {
        std::string empty = ".EMPTY";
        Node emptyNode = Node(empty, empty);
        newParent.addChild(emptyNode);
    }

    treeDeque.push_back(newParent);

    return currState;

}

void handleFail(int k) {
    std::cerr << "ERROR at " << k << std::endl;
}

void printTree(Node &root) {
    
    if (root.isLeaf()) {
        if (root.getType() == ".EMPTY") {
            return;
        }
        std::cout << root.getType() << " " << root.getLexeme() << std::endl;
    } else {
        std::cout << root.getProdRule() << std::endl;
        for (int i = 0; i < root.children.size(); i++) {
            printTree(root.children[root.children.size() - i - 1]);
        }
    }
}

int main() {

    int maxState = 0;

    std::map<int, State> states;

    std::string line = "";

    std::vector<std::pair<std::string, std::string>> derivations;

    std::deque<std::pair<std::string,std::string>> inputSequence;
    std::deque<int> stateDeque;
    std::deque<Node> treeDeque;
    std::string inputString = "";
    int k = 1;

    inputSequence.push_back(std::make_pair("BOF", "BOF"));

    // take in std input
    while (getline(std::cin, line)) {
        auto lineSplit = split(line);
        inputSequence.push_back(std::make_pair(lineSplit[0], lineSplit[1]));
    }

    inputSequence.push_back(std::make_pair("EOF", "EOF"));

    bool fillDerivations = false;
    bool fillTransitions = false;
    bool fillReductions = false;

    std::stringstream wlp4Stream(WLP4_COMBINED);

    while (getline(wlp4Stream, line)) {

        if (line == ".CFG") {
            fillDerivations = true;
            continue;
        } else if (line == ".TRANSITIONS") {
            fillDerivations = false;
            fillTransitions = true;
            continue;
        } else if (line == ".REDUCTIONS") {
            fillTransitions = false;
            fillReductions = true;
            continue;

        } else if (line == ".END") {
            // process
            int currState = 0;

            stateDeque.push_back(0);

            bool endProcess = false;

            while (true) {
                
                if (endProcess) {
                    printTree(treeDeque[0]);
                    return 0;
                }

                std::string next = "";

                if (states[currState].checkAccept()) {
                    next = ".ACCEPT";
                    endProcess = true;
                } else if (inputSequence.size() == 0) {
                    handleFail(k);
                    return 0;
                } else {
                    next = inputSequence.front().first;
                }

                int reduceNum = states[currState].needReduce(next, derivations.size());

                if (reduceNum == -1) { // shift

                    shift(inputSequence, treeDeque, derivations, k);

                } else { // reduce
                    std::pair<std::string, std::string> derivation = derivations[reduceNum];
                    currState = reduce(reduceNum, derivations, treeDeque, stateDeque);
                    if (endProcess) continue;
                    next = derivation.first;
                }

                if (states[currState].hasTransition(next)) {
                    currState = states[currState].getNextState(next);
                    stateDeque.push_back(currState);
                } else {

                    if (inputSequence.size() > 0) k--;
                    
                    handleFail(k);
                    
                    return 0;
                }
            }

            return 0;
        }

        if (fillDerivations) {
            std::pair<std::string,std::string> lineSplit = splitByFirstSpace(line);
            derivations.push_back(std::make_pair(lineSplit.first, lineSplit.second));
        }

        if (fillTransitions) {

            auto splitTransition = split(line);
            int startState = std::stoi(splitTransition[0]);
            std::string transitionString = splitTransition[1];
            int endState = std::stoi(splitTransition[2]);
            if (states.count(startState) == 0) {
                states.insert({startState, State(startState, std::make_pair(transitionString, endState))});
            } else {
                states.at(startState).addTransition(std::make_pair(transitionString, endState));
            }
            
        }

        if (fillReductions) {

            auto splitReduction = split(line);
            int stateId = std::stoi(splitReduction[0]);
            int derivationIdx = std::stoi(splitReduction[1]);
            std::string reduction = splitReduction[2];

            states[stateId].addReduction(derivationIdx, reduction);
        }
    }

    return 0;
}