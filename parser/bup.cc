#include <iostream>
#include <map>
#include <vector>
#include <deque>
#include <algorithm>

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

int main() {

    std::string line = "";

    std::vector<std::pair<std::string, std::string>> derivations;

    std::deque<std::string> inputSequence;
    std::vector<std::string> reductionSequence;
    std::string inputString = "";

    bool fillDerivations = false;
    bool fillInput = false;
    bool fillActions = false;

    while (getline(std::cin, line)) {

        if (line == ".CFG") {
            fillDerivations = true;
            continue;
        } else if (line == ".INPUT") {
            fillDerivations = false;
            fillInput = true;
            continue;
        } else if (line == ".ACTIONS") {
            auto inputSequenceVec = split(inputString);
            for (auto element : inputSequenceVec) {
                inputSequence.push_back(element);
            }
            fillInput = false;
            fillActions = true;
            continue;
        } else if (line == ".END") {
            return 0;
        }

        if (fillDerivations) {
            std::pair<std::string,std::string> lineSplit = splitByFirstSpace(line);
            derivations.push_back(std::make_pair(lineSplit.first, lineSplit.second));
        }

        if (fillInput) {
            removeEndSpace(line);
            removeStartSpace(line);
            if (inputString.size() != 0) inputString += " ";
            inputString += line;
        }

        if (fillActions) {

            std::pair<std::string,std::string> splitLine = splitByFirstSpace(line);

            if (splitLine.first == "shift") {

                std::string firstElement = inputSequence[0];

                inputSequence.pop_front();
                
                reductionSequence.push_back(firstElement);

            } else if (splitLine.first == "reduce") {

                int reduceNum = std::stoi(splitLine.second);
                std::pair<std::string, std::string> derivation = derivations[reduceNum];
                if (derivation.second != ".EMPTY") {

                    std::vector<std::string> rhsDerivation = split(derivation.second);
                    int i2 = reductionSequence.size() - 1;
                    
                    for (int i = rhsDerivation.size() - 1; i >= 0; i--) {
                        if (reductionSequence[i2] == rhsDerivation[i]) {
                            i2--;
                            reductionSequence.pop_back();
                        } else {
                            break;
                        }
                    }

                    reductionSequence.push_back(derivation.first);

                } else {
                    reductionSequence.push_back(derivation.first);
                }

            } else if (splitLine.first == "print") {

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
        }
    }

    return 0;
}