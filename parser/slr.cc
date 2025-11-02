#include <iostream>
#include <bits/stdc++.h>

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

std::pair<std::string,std::string> splitByFirstSpace(std::string &line) {
    int spaceIdx = line.find(' ');
    std::string first = line.substr(0, spaceIdx);
    std::string second = "";
    if (spaceIdx < line.size()) {
        second = line.substr(spaceIdx + 1);
    }
    return std::make_pair(first,second);
}

std::string removeStartSpace(std::string code) {

    code.erase(code.begin(), std::find_if(code.begin(), code.end(), [](unsigned char c) {
        return !std::isspace(c);
    }));

    return code;
}

std::string removeEndSpace(std::string code) {
    code.erase(std::find_if(code.rbegin(), code.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), code.end());

    return code;
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

bool isNonTerminal(std::string symbol, std::set<std::string> nonTerminals) {
    return nonTerminals.count(symbol) > 0;
}

void shift(std::deque<std::string> &inputSequence, std::vector<std::string> &reductionSequence, std::vector<std::pair<std::string, std::string>> &derivations) {
    std::string firstElement = inputSequence[0];
    inputSequence.pop_front(); 
    reductionSequence.push_back(firstElement);
}

int reduce(int reduceNum, std::vector<std::pair<std::string, std::string>> &derivations, std::vector<std::string> &reductionSequence, std::deque<int> &stateDeque) {

    int currState = stateDeque.back();

    std::pair<std::string, std::string> derivation = derivations[reduceNum];

    if (derivation.second != ".EMPTY") {

        std::vector<std::string> rhsDerivation = split(derivation.second);
        int i2 = reductionSequence.size() - 1;
        
        for (int i = rhsDerivation.size() - 1; i >= 0; i--) {
            if (reductionSequence[i2] == rhsDerivation[i]) {
                i2--;
                reductionSequence.pop_back();
                stateDeque.pop_back();
            } else {
                break;
            }
        }
        
        currState = stateDeque.back();
    }

    reductionSequence.push_back(derivation.first);

    return currState;

}

void print(std::deque<std::string> &inputSequence, std::vector<std::string> &reductionSequence) {
    for (auto token : reductionSequence) {
        std::cout << token << " ";
    }
    std::cout << ".";
    if (inputSequence.size() > 0) std::cout << " ";
    for (int i = 0; i < inputSequence.size(); i++) {
        std::string end = " ";
        if (i == inputSequence.size() - 1) end = "";
        std::cout << inputSequence[i] << end;
    }
    std::cout << std::endl;
}

void handleFail(int k) {
    std::cerr << "ERROR at " << k << std::endl;
}

int main() {

    int maxState = 0;

    std::map<int, State> states;

    std::string line = "";

    std::vector<std::pair<std::string, std::string>> derivations;

    std::deque<std::string> inputSequence;
    std::deque<int> stateDeque;
    std::vector<std::string> reductionSequence;
    std::set<std::string> nonTerminals;
    std::string inputString = "";
    int k = 0;

    bool fillDerivations = false;
    bool fillInput = false;
    bool fillTransitions = false;
    bool fillReductions = false;

    while (getline(std::cin, line)) {

        if (line == ".CFG") {
            fillDerivations = true;
            continue;
        } else if (line == ".INPUT") {
            fillDerivations = false;
            fillInput = true;
            continue;
        } else if (line == ".TRANSITIONS") {
            auto inputSequenceVec = split(inputString);
            for (auto element : inputSequenceVec) {
                inputSequence.push_back(element);
            }
            fillInput = false;
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

                print(inputSequence, reductionSequence);
                
                if (endProcess) return 0;

                std::string next = "";

                if (states[currState].checkAccept()) {
                    next = ".ACCEPT";
                    endProcess = true;
                } else if (inputSequence.size() == 0) {
                    handleFail(k);
                    return 0;
                } else {
                    next = inputSequence.front();
                }

                int reduceNum = states[currState].needReduce(next, derivations.size());

                if (reduceNum == -1) { // shift

                    shift(inputSequence, reductionSequence, derivations);

                    if (!isNonTerminal(reductionSequence.back(), nonTerminals)) k++;
                    
                    if (states[currState].hasTransition(next)) {
                        currState = states[currState].getNextState(next);
                        stateDeque.push_back(currState);
                    } else {
                        handleFail(k);
                        return 0;
                    }

                } else { // reduce

                    std::pair<std::string, std::string> derivation = derivations[reduceNum];

                    currState = reduce(reduceNum, derivations, reductionSequence, stateDeque);

                    if (endProcess) continue;

                    if (states[currState].hasTransition(derivation.first)) {
                        currState = states[currState].getNextState(derivation.first);
                        stateDeque.push_back(currState);
                    } else {
                        handleFail(k);
                        return 0;
                    }
                }
            }

            return 0;
        }

        if (fillDerivations) {
            std::pair<std::string,std::string> lineSplit = splitByFirstSpace(line);
            derivations.push_back(std::make_pair(lineSplit.first, lineSplit.second));
            nonTerminals.insert(lineSplit.first);
        }

        if (fillInput) {
            removeEndSpace(line);
            removeStartSpace(line);
            if (inputString.size() != 0) inputString += " ";
            inputString += line;
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